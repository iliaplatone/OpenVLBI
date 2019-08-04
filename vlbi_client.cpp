#include "vlbi_client.h"

static int is_running = 1;
char lockfile[150] = "/tmp/openvlbi.lock";

VLBI::Client::Client()
{
	f = stdout;
	Ra=0;
       	Dec = 0;
        Freq = 1420000000;
        SampleRate = 100000000;
        Bps = 8;
        Gain = 1;
        Bandwidth = 10000;
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

void VLBI::Client::AddNode(char *name, double x, double y, double z, unsigned char *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	dsp_buffer_copy(buf, node->buf, len);
	z = vlbi_calc_elevation_coarse(z, y);
	node->location[0] = x;
	node->location[1] = y;
	node->location[2] = z;
	node->starttimeutc = starttime;
	vlbi_add_stream(context, node, name);
}

void VLBI::Client::AddNode(char *name, double x, double y, double z, unsigned short int *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	dsp_buffer_copy(buf, node->buf, len);
	z = vlbi_calc_elevation_coarse(z, y);
	node->location[0] = x;
	node->location[1] = y;
	node->location[2] = z;
	node->starttimeutc = starttime;
	vlbi_add_stream(context, node, name);
}

void VLBI::Client::AddNode(char *name, double x, double y, double z, unsigned int *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	dsp_buffer_copy(buf, node->buf, len);
	z = vlbi_calc_elevation_coarse(z, y);
	node->location[0] = x;
	node->location[1] = y;
	node->location[2] = z;
	node->starttimeutc = starttime;
	vlbi_add_stream(context, node, name);
}

void VLBI::Client::AddNode(char *name, double x, double y, double z, unsigned long int *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	dsp_buffer_copy(buf, node->buf, len);
	z = vlbi_calc_elevation_coarse(z, y);
	node->location[0] = x;
	node->location[1] = y;
	node->location[2] = z;
	memcpy(&node->starttimeutc, &starttime, sizeof(timespec));
	vlbi_add_stream(context, node, name);
}

void VLBI::Client::AddNode(char *name, double x, double y, double z, float *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	dsp_buffer_copy(buf, node->buf, len);
	z = vlbi_calc_elevation_coarse(z, y);
	node->location[0] = x;
	node->location[1] = y;
	node->location[2] = z;
	node->starttimeutc = starttime;
	vlbi_add_stream(context, node, name);
}

void VLBI::Client::AddNode(char *name, double x, double y, double z, double *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	dsp_stream_add_dim(node, len);
	dsp_stream_alloc_buffer(node, len);
	dsp_buffer_copy(buf, node->buf, len);
	z = vlbi_calc_elevation_coarse(z, y);
	node->location[0] = x;
	node->location[1] = y;
	node->location[2] = z;
	node->starttimeutc = starttime;
	vlbi_add_stream(context, node, name);
}

void VLBI::Client::DelNode(char *name)
{
	vlbi_del_stream(context, name);
}

dsp_stream_p VLBI::Client::GetPlot(int u, int v, int type)
{
	dsp_stream_p plot;
	double coords[3] = { Ra, Dec };
	if(type&EARTH_TIDE) {
		plot = vlbi_get_uv_plot_earth_tide(context, ((type&GEOCENTRIC_COORDS) ? 0 : 1), u, v, coords, Freq, SampleRate);
	} else {
		plot = vlbi_get_uv_plot_moving_baseline(context, ((type&GEOCENTRIC_COORDS) ? 0 : 1), u, v, coords, Freq, SampleRate);
	}
	dsp_buffer_stretch(plot->buf, plot->len, 0.0, 1.0);
	if((type&DFT) && (plot != NULL)) {
		plot = vlbi_get_fft_estimate(plot);
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
            Bps = (int)strtol(value, NULL, 10);
        }
        else if(!strcmp(arg, "bandwidth")) {
            Bandwidth = (double)atof(value);
        }
        else if(!strcmp(arg, "model")) {
        }
    }
    else if(!strcmp(cmd, "get")) {
        if(!strcmp(arg, "observation")) {
            int type = 0;
            if(!strcmp(value, "earth_tide_raw_geo")) {
                type |= GEOCENTRIC_COORDS;
                type |= EARTH_TIDE;
            }
            else if(!strcmp(value, "earth_tide_dft_geo")) {
                type |= GEOCENTRIC_COORDS;
                type |= DFT;
                type |= EARTH_TIDE;
            }
            else if(!strcmp(value, "earth_tide_raw_abs")) {
                type |= EARTH_TIDE;
            }
            else if(!strcmp(value, "earth_tide_dft_abs")) {
                type |= DFT;
                type |= EARTH_TIDE;
            }
            else if(!strcmp(value, "moving_base_raw_geo")) {
                type |= GEOCENTRIC_COORDS;
            }
            else if(!strcmp(value, "moving_base_dft_geo")) {
                type |= DFT;
                type |= GEOCENTRIC_COORDS;
            }
            else if(!strcmp(value,  "moving_base_raw_abs")) {
            }
            else if(!strcmp(value, "moving_base_dft_abs")) {
                type |= DFT;
            }
            dsp_stream_p plot = GetPlot(w, h, type);
            if (plot != NULL) {
                dsp_buffer_stretch(plot->buf, plot->len, 0.0, 255.0);
                int ilen = plot->len*sizeof(double);
                int olen = ilen*4/3+4;
                char* base64 = (char*)malloc(olen);
                to64frombits((unsigned char*)base64, (unsigned char*)plot->buf, ilen);
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
            char name[32];
            double lat, lon, el;
            char* k = strtok(value, ",");
            strcpy(name, k);
            k = strtok(NULL, ",");
            lat = (double)atof(k);
            k = strtok(NULL, ",");
            lon = (double)atof(k);
            k = strtok(NULL, ",");
            el = (double)atof(k);
            k = strtok(NULL, ",");
            FILE *tmp = fopen(k, "r");
            fseek(tmp, 0, SEEK_END);
            int ilen = ftell(tmp);
            int olen = ilen*3/4;
            fseek(tmp, 0, SEEK_SET);
            char *base64 = (char*)malloc(ilen);
            unsigned char *buf = (unsigned char*)malloc(olen);
            fread(base64, 1, ilen, tmp);
            fclose(tmp);
            from64tobits_fast((char*)buf, (char*)base64, ilen);
            k = strtok(NULL, ",");
            switch(Bps) {
                case 8:
                    AddNode(name, lat, lon, el, (unsigned char*)buf, olen, vlbi_time_string_to_utc(k));
                    break;
                case 16:
                    AddNode(name, lat, lon, el, (unsigned short int*)buf, olen, vlbi_time_string_to_utc(k));
                    break;
                case 32:
                    AddNode(name, lat, lon, el, (unsigned int*)buf, olen, vlbi_time_string_to_utc(k));
                    break;
                case 64:
                    AddNode(name, lat, lon, el, (unsigned long int*)buf, olen, vlbi_time_string_to_utc(k));
                    break;
                case -32:
                    AddNode(name, lat, lon, el, (float*)buf, olen, vlbi_time_string_to_utc(k));
                    break;
                case -64:
                    AddNode(name, lat, lon, el, (double*)buf, olen, vlbi_time_string_to_utc(k));
                    break;
                default:
                    fprintf(stderr, "No compatible bits per sample value");
                    break;
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
    char cmd[32], arg[32], value[4032];
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGSTOP, sighandler);
    signal(SIGQUIT, sighandler);
    while (is_running) {
        if(3 != fscanf(stdin, "%s %s %s", cmd, arg, value)) { if (!strcmp(cmd, "quit")) is_running=0; continue; }
	client->Parse(cmd, arg, value);
    }
    return 0;
}
