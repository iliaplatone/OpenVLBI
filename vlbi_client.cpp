#include "vlbi_client.h"

static double* correlation_func(double d1, double d2)
{
    static double res = 0;
    res = d1 * d2;
    return &res;
}

VLBI::Client::Client()
{
	Ra=0;
       	Dec = 0;
        Freq = 1420000000;
        SampleRate = 100000000;
        Bps = 8;
        Gain = 1;
        Bandwidth = 10000;
        w = 128;
        h = 128;
	num_contexts = 0;
	contexts = (_vlbi_context_p*)malloc(sizeof(_vlbi_context_p));
	model = dsp_stream_new();
	uv = dsp_stream_new();
	fft = dsp_stream_new();
}

VLBI::Client::~Client()
{
    if(context != NULL) {
        vlbi_exit(context);
    }
}

void VLBI::Client::AddNode(double lat, double lon, double el, double *buf, double len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	node->location[0] = lat;
	node->location[1] = lon;
	node->location[2] = el;
	node->location[2] = vlbi_calc_elevation_coarse(node->location[2], node->location[0]);
	node->target[0] = Ra;
	node->target[1] = Dec;
	dsp_stream_add_dim(node, len);
	dsp_stream_set_buffer(node, (void*)(buf), node->len);
	node->starttimeutc = starttime;
	vlbi_add_stream(context, node);
}

void VLBI::Client::Parse(char* cmd, char* arg, char* value)
{
        if(!strcmp(cmd, "set")) {
            if(!strcmp(arg, "context")) {
                int i = 0;
                while(strncmp(contexts[i]->name, value, strlen(value)) != 0 && i < num_contexts) {
                    i++;
                }
                if(i > 0 && !strcmp(contexts[i]->name, value)) {
                    SetContext(contexts[i]->ctx);
                }
            }
            else if(!strcmp(arg, "resolution")) {
                char* W = strtok(value, ",");
                char* H = strtok(NULL, ",");
                w = (double)strtol(W, NULL, 10);
                h = (double)strtol(H, NULL, 10);
            }
            else if(!strcmp(arg, "target")) {
                char* ra = strtok(value, ",");
                char* dec = strtok(NULL, ",");
                Ra = (double)atof(ra);
                Dec = (double)atof(dec);
            }
            else if(!strcmp(arg, "frequency")) {
                Freq = (double)atof(value);
            }
            else if(!strcmp(arg, "samplerate")) {
                SampleRate = (double)atof(value);
            }
            else if(!strcmp(arg, "bitspersample")) {
                Bps = (double)atof(value);
            }
            else if(!strcmp(arg, "bandwidth")) {
                Bandwidth = (double)atof(value);
            }
            else if(!strcmp(arg, "model")) {
            }
        }
        else if(!strcmp(cmd, "add")) {
            if(!strcmp(arg, "context")) {
                contexts = (_vlbi_context_p*)realloc(contexts, sizeof(_vlbi_context_p)*(num_contexts+1));
                contexts[num_contexts] = (_vlbi_context_p)malloc(sizeof(_vlbi_context_t));
                contexts[num_contexts]->ctx = vlbi_init();
                contexts[num_contexts]->name = (char*)malloc(strlen(value));
                strcpy(contexts[num_contexts]->name, value);
                num_contexts++;
            }
            if(!strcmp(arg, "node")) {
		double lat, lon, el, *buf, Y, M, D, H, m, s;
		char* k = strtok(value, ",");
		lat = (double)atof(k);
		k = strtok(NULL, ",");
		lon = (double)atof(k);
		k = strtok(NULL, ",");
		el = (double)atof(k);
		k = strtok(NULL, ",");
		FILE *tmp = fopen(k, "r");
		fseek(tmp, 0, SEEK_END);
		int buflen = ftell(tmp);
		rewind(tmp);
		char *data = (char*)malloc(buflen);
		fread(data, 1, buflen, tmp);
		fclose(tmp);
		int len = strlen(k)*3;
		buf = (double*)malloc(len);
		buf = (double*)from64tobits_fast(k, (char*)buf, buflen);
		k = strtok(NULL, "/");
		Y = (double)atof(k);
		k = strtok(NULL, "/");
		M = (double)atof(k);
		k = strtok(NULL, "/");
		D = (double)atof(k);
		k = strtok(NULL, "-");
		H = (double)atof(k);
		k = strtok(NULL, ":");
		m = (double)atof(k);
		k = strtok(NULL, ":");
		s = (double)atof(k);
		AddNode(lat, lon, el, buf, len, vlbi_time_mktimespec(Y, M, D, H, m, floor(s), (s - floor(s))*1000000000.0));
            }
        }
        else if(!strcmp(cmd, "get")) {
            if(!strcmp(arg, "coordinate")) {
		    if(!strcmp(arg, "ra")) {
                        fprintf(stdout, "%02.05lf", Ra);
		    }
		    if(!strcmp(arg, "dec")) {
                        fprintf(stdout, "%02.05lf", Dec);
		    }
	    }
            if(!strcmp(arg, "observation")) {
                double coords[] =  { Ra, Dec };
                uv = vlbi_get_uv_plot_earth_tide(GetContext(), (vlbi_func2_t)correlation_func, 0, w, h, coords, Freq, SampleRate);
                if(!strcmp(value, "fft")) {
                    if (uv != NULL) {
                        dsp_stream_p fft = vlbi_get_fft_estimate(uv);
                        if (fft != NULL) {
                            unsigned int len = fft->len * 4 / 3 + 4;
                            unsigned char* base64 = (unsigned char*)malloc(len);
                            to64frombits(base64, (unsigned char*)fft->buf, fft->len);
                            if(len<fwrite(base64, 1, len, stdout))return;
                            free(base64);
                            dsp_stream_free(fft);
                        }
                        dsp_stream_free(uv);
                    }
                }
                else if(!strcmp(value, "mdl")) {
                    if (uv != NULL && model != NULL) {
                        dsp_stream_p fft = vlbi_apply_model(uv, model);
                        if (fft != NULL) {
                            dsp_stream_p res = vlbi_get_fft_estimate(fft);
                            if (res != NULL) {
                                unsigned char* base64 = (unsigned char*)malloc(fft->len * 4 / 3 + 4);
                                to64frombits(base64, (unsigned char*)fft->buf, fft->len);
                                fwrite(base64, 1, uv->len * 4 / 3 + 4, stdout);
                                free(base64);
                                dsp_stream_free(res);
                            }
                            dsp_stream_free(fft);
                        }
                        dsp_stream_free(uv);
                    }
                }
                else if(!strcmp(value, "raw")) {
                    if (uv != NULL) {
                        unsigned char* base64 = (unsigned char*)malloc(uv->len * 4 / 3 + 4);
                        to64frombits(base64, (unsigned char*)uv->buf, uv->len);
                        fwrite(base64, 1, uv->len * 4 / 3 + 4, stdout);
                        free(base64);
                        dsp_stream_free(uv);
                    }
                }
            }
        }
}

extern VLBI::Client *client;
int is_running = 1;

void sighandler(int signum)
{
    signal(signum, SIG_IGN);
    client->~Client();
    signal(signum, sighandler);
    exit(0);
}

int main(int argc, char** argv)
{
    FILE* ifile = stdin;

    char cmd[150], arg[150], value[150];
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGSTOP, sighandler);
    signal(SIGQUIT, sighandler);
    while (is_running) {
        if(fscanf(ifile, "%s %s %s", cmd, arg, value) != 3) continue;
	client->Parse(cmd, arg, value);
    }
    return 0;
}
