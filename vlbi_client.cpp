#include "vlbi_client.h"

static int is_running = 1;

VLBI::Client::Client()
{
	f = stdout;
	Ra=0;
    Dec = 0;
    Freq = 1420000000;
    SampleRate = 100000000;
    Bps = 8;
    Gain = 1;
    w = 128;
    h = 128;
	contexts = new InstanceCollection();
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

void VLBI::Client::AddNode(char *name, double x, double y, double z, void *buf, int bytelen, timespec starttime, bool geo)
{
	dsp_stream_p node = dsp_stream_new();
	int len = bytelen*8/abs(Bps);
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	switch(Bps) {
		case 16:
		dsp_buffer_copy(((unsigned short int*)buf), node->buf, len);
		break;
		case 32:
		dsp_buffer_copy(((unsigned int*)buf), node->buf, len);
		break;
		case 64:
		dsp_buffer_copy(((unsigned long int*)buf), node->buf, len);
		break;
		case -32:
		dsp_buffer_copy(((float*)buf), node->buf, len);
		break;
		case -64:
		dsp_buffer_copy(((double*)buf), node->buf, len);
		break;
		case 8:
		dsp_buffer_copy(((unsigned char*)buf), node->buf, len);
		break;
		default:
		break;
	}
    node->location->xyz.x = x;
    node->location->xyz.y = y;
    node->location->xyz.z = z;
	memcpy(&node->starttimeutc, &starttime, sizeof(timespec));
    vlbi_add_stream(context, node, name, geo);
}

void VLBI::Client::DelNode(char *name)
{
	vlbi_del_stream(context, name);
}

dsp_stream_p VLBI::Client::GetPlot(int u, int v, int type)
{
	dsp_stream_p plot;
	double coords[3] = { Ra, Dec };
    if(type&UV_COVERAGE) {
        if(type&APERTURE_SYNTHESIS) {
            plot = vlbi_get_uv_plot_aperture_synthesis(context, u, v, coords, Freq, SampleRate);
        } else {
            plot = vlbi_get_uv_plot_moving_baseline(context, u, v, coords, Freq, SampleRate);
        }
    } else {
        if(type&APERTURE_SYNTHESIS) {
            plot = vlbi_get_uv_plot_aperture_synthesis(context, u, v, coords, Freq, SampleRate);
        } else {
            plot = vlbi_get_uv_plot_moving_baseline(context, u, v, coords, Freq, SampleRate);
        }
	}
	return plot;
}

void VLBI::Client::Parse(char* cmd, char* arg, char* value)
{
    if(!strcmp(cmd, "set")) {
        if(!strcmp(arg, "context")) {
            if(contexts->ContainsKey(value)) {
                SetContext(contexts->Get(value));
            }
        }
        else if(!strcmp(arg, "resolution")) {
            char* W = strtok(value, "x");
            char* H = strtok(NULL, "x");
            w = (int)atof(W);
            h = (int)atof(H);
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
            Bps = (int)atof(value);
        }
        else if(!strcmp(arg, "model")) {
        }
    }
    else if(!strcmp(cmd, "get")) {
        if(!strcmp(arg, "observation")) {
            int type = 0;
            char *t = strtok(value, "_");
            if(!strcmp(t, "synthesis")) {
                type |= APERTURE_SYNTHESIS;
            } else if(!strcmp(t, "movingbase")) {
                type &= ~APERTURE_SYNTHESIS;
            }
            dsp_stream_p plot = GetPlot(w, h, type);
            if (plot != NULL) {
                t = strtok(NULL, "_");
                if(!strcmp(t, "idft")) {
                    vlbi_get_ifft_estimate(plot);
                } else if(!strcmp(t, "coverage")) {
                    for(int i = 0; i < plot->len; i++)
                    {
                        if(plot->buf[i] != 0.0)
                            plot->buf[i] = 1;
                    }
                }
                dsp_buffer_stretch(plot->buf, plot->len, 0.0, 255.0);
                int ilen = plot->len;
                int olen = ilen*4/3+4;
                unsigned char* buf = (unsigned char*)malloc(plot->len);
                dsp_buffer_copy(plot->buf, buf, plot->len);
                char* base64 = (char*)malloc(olen);
                to64frombits((unsigned char*)base64, (unsigned char*)buf, ilen);
                fwrite(base64, 1, olen, f);
                free(base64);
                dsp_stream_free(plot);
            }
        }
    }
    else if(!strcmp(cmd, "add")) {
        if(!strcmp(arg, "context")) {
            if(!contexts->ContainsKey(value)) {
                contexts->Add(vlbi_init(), value);
            }
        }
        else if(!strcmp(arg, "node")) {
            char name[32], file[150], date[64];
            double lat, lon, el;
            int geo = 0;
            char* k = strtok(value, ",");
            strcpy(name, k);
            k = strtok(NULL, ",");
            geo = strcmp(k, "geo") == 0 ? 1 : (strcmp(k, "xyz") ? 2 : 0);
            k = strtok(NULL, ",");
            lat = (double)atof(k);
            k = strtok(NULL, ",");
            lon = (double)atof(k);
            k = strtok(NULL, ",");
            el = (double)atof(k);
            k = strtok(NULL, ",");
            strcpy(file, k);
            k = strtok(NULL, ",");
            strcpy(date, k);
            void *buf = malloc(1);
            int len = vlbi_b64readfile(file, buf);
            if(len > 0 && geo > 0) {
                AddNode(name, lat, lon, el, buf, len, vlbi_time_string_to_utc(date), geo == 1);
            }
        }
    }
    else if(!strcmp(cmd, "del")) {
        if(!strcmp(arg, "node")) {
            DelNode(value);
        }
        else if(!strcmp(arg, "context")) {
            contexts->RemoveKey(value);
        }
    }
}

extern VLBI::Client *client;

static void sighandler(int signum)
{
    signal(signum, SIG_IGN);
    client->~Client();
    signal(signum, sighandler);
    exit(0);
}

int main(int argc, char** argv)
{
    char cmd[32], arg[32], value[4032], opt;
    while ((opt = getopt(argc, argv, "h:")) != -1) {
        switch (opt) {
            case 'h':
                vlbi_max_threads((int)atof(optarg));
                break;
            default:
            fprintf(stderr, "Usage: %s [-h max_threads]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGSTOP, sighandler);
    signal(SIGQUIT, sighandler);
    while (is_running) {
        if(3 != fscanf(stdin, "%s %s %s", cmd, arg, value)) { if (!strcmp(cmd, "quit")) is_running=0; continue; }
	client->Parse(cmd, arg, value);
    }
    return EXIT_SUCCESS;
}
