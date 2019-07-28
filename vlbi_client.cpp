#include "vlbi_client.h"

static int is_running = 1;
char lockfile[150];

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

void VLBI::Client::AddNode(double lat, double lon, double el, double *buf, int len, timespec starttime)
{
	dsp_stream_p node = dsp_stream_new();
	node->location[0] = lat;
	node->location[1] = lon;
	node->location[2] = el;
	node->location[2] = vlbi_calc_elevation_coarse(node->location[2], node->location[0]);
	node->target[0] = Ra;
	node->target[1] = Dec;
	node->starttimeutc = starttime;
	dsp_stream_add_dim(node, len);
	dsp_stream_set_buffer(node, (void*)(buf), len);
	vlbi_add_stream(context, node);
}

dsp_stream_p VLBI::Client::GetPlot(int u, int v, plot_type_t type)
{
	dsp_stream_p plot;
	bool earth_tide = ((type&EARTH_TIDE)==EARTH_TIDE)?true:false;
	bool geodetic_coords = ((type&GEODETIC_COORDS)==GEODETIC_COORDS)?true:false;
	bool dft = ((type&DFT)==DFT)?true:false;
	double coords[3] = { Ra, Dec };
	if(earth_tide) {
		plot = vlbi_get_uv_plot_earth_tide(context, (geodetic_coords ? 0 : 1), u, v, coords, Freq, SampleRate);
	} else {
		plot = vlbi_get_uv_plot_moving_baseline(context, (geodetic_coords ? 0 : 1), u, v, coords, Freq, SampleRate);
	}
	if(dft && (plot != NULL)) {
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
                if(!contexts->ContainsKey(value)) {
                    contexts->Add(vlbi_init(), value);
                }
            }
            else if(!strcmp(arg, "node")) {
		fprintf(stdout, "adding node");
		double lat, lon, el;
		char* k = strtok(value, ",");
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
		double *buf = (double*)malloc(olen);
		fread(base64, 1, ilen, tmp);
		fclose(tmp);
		from64tobits_fast((char*)buf, (char*)base64, ilen);
		k = strtok(NULL, ",");
		AddNode(lat, lon, el, buf, olen/8, vlbi_time_string_to_utc(k));
            }
        }
        else if(!strcmp(cmd, "get")) {
            if(!strcmp(arg, "observation")) {
		plot_type_t type = earth_tide_dft_geo;
                if(!strcmp(value, "earth_tide_raw_geo")) {
                    type = earth_tide_dft_geo;
                }
                else if(!strcmp(value, "earth_tide_dft_geo")) {
                    type = earth_tide_dft_geo;
                }
                else if(!strcmp(value, "earth_tide_raw_abs")) {
                    type = earth_tide_raw_abs;
                }
                else if(!strcmp(value, "earth_tide_dft_abs")) {
                    type = earth_tide_dft_abs;
                }
                else if(!strcmp(value, "moving_base_raw_geo")) {
                    type = moving_base_raw_geo;
                }
                else if(!strcmp(value, "moving_base_dft_geo")) {
                    type = moving_base_dft_geo;
                }
                else if(!strcmp(value,  "moving_base_raw_abs")) {
                    type = moving_base_raw_abs;
                }
                else if(!strcmp(value, "moving_base_dft_abs")) {
                    type = moving_base_dft_abs;
                }
                dsp_stream_p plot = GetPlot(w, h, type);
                if (plot != NULL) {
                    int olen = plot->len*sizeof(double)*4/3+4;
                    char* base64 = (char*)malloc(olen);
                    to64frombits((unsigned char*)base64, (unsigned char*)plot->buf, plot->len*sizeof(double));
                    fwrite(base64, 1, olen, f);
                    free(base64);
                    dsp_stream_free(plot);
                }
            }
        }
	putchar('+');
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
    strcpy(lockfile, argv[1]);
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGSTOP, sighandler);
    signal(SIGQUIT, sighandler);
    char c = 0;
    while (is_running) {
        if(3 != fscanf(stdin, "%s %s %s", cmd, arg, value)) continue;
        creat(lockfile, 700);
	client->Parse(cmd, arg, value);
        unlink(lockfile);
    }
    return 0;
}
