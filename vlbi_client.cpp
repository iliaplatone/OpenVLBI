#include "vlbi_client.h"
#include <cstring>

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

static double fillone_delegate(double x, double y) {
    return 1.0;
}

void VLBI::Client::AddNode(char *name, dsp_location *locations, void *buf, int bytelen, timespec starttime, bool geo)
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
    node->location = locations;
    memcpy(&node->starttimeutc, &starttime, sizeof(timespec));
    vlbi_add_node(context, node, name, geo);
}

void VLBI::Client::DelNode(char *name)
{
    vlbi_del_node(context, name);
}

dsp_stream_p VLBI::Client::GetPlot(int u, int v, int type, bool nodelay)
{
    dsp_stream_p plot;
    double coords[3] = { Ra, Dec };
    plot = vlbi_get_uv_plot(GetContext(), u, v, coords, Freq, SampleRate, nodelay, (type&APERTURE_SYNTHESIS) == 0, (type&UV_COVERAGE) != 0 ? fillone_delegate : vlbi_default_delegate);
    return plot;
}

void VLBI::Client::Parse(FILE* f)
{
    size_t len = 0;
    char *cmd = nullptr;
    char *arg = nullptr;
    char *value = nullptr;
    char *str = nullptr;
    getdelim(&str, &len, (int)'\n', f);
    *strrchr(str, '\n') = 0;
    if(str == nullptr)
        return;
    if (len == 0)
        return;
    str[len-2] = 0;
    cmd = strtok(str, " ");
    if (cmd == nullptr)
        return;
    if (!strcmp(cmd, "quit")) {
        is_running = false;
        return;
    } else {
        arg = strtok(NULL, " ");
        if (arg == nullptr)
            return;
        value = strtok(NULL, " ");
        if(value == nullptr)
            return;
        if(!strcmp(cmd, "set")) {
            if(!strcmp(arg, "context")) {
                SetContext(value);
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
            else if(!strcmp(arg, "location")) {
                double lat, lon, el;
                char *t = strtok(value, ",");
                lat = (int)atof(t);
                t = strtok(NULL, ",");
                lon = (int)atof(t);
                t = strtok(NULL, ",");
                el = (int)atof(t);
                vlbi_set_location(GetContext(), lat, lon, el);
            }
            else if(!strcmp(arg, "model")) {
            }
        }
        else if(!strcmp(cmd, "get")) {
            if(!strcmp(arg, "nodes")) {
                vlbi_node* nodes;
                int n = vlbi_get_nodes(GetContext(), &nodes);
                for(int x = 0; x < n; x++) {
                    fprintf(f, "Node #%d: name:%s relative?:%s x:%lf y:%lf z:%lf latitude:%lf longitude:%lf elevation:%lf\n", nodes[x].Index, nodes[x].Name, nodes[x].Geo ? "no" : "yes", nodes[x].Location[0], nodes[x].Location[1], nodes[x].Location[2], nodes[x].GeographicLocation[0], nodes[x].GeographicLocation[1], nodes[x].GeographicLocation[2]);
                }
                free(nodes);
            } else if(!strcmp(arg, "baselines")) {
                vlbi_baseline* baselines;
                int n = vlbi_get_baselines(GetContext(), &baselines);
                for(int x = 0; x < n; x++) {
                    fprintf(f, "Baseline #%d: name:%s samplerate:%lf wavelength:%lf custom?:%s\n", x, baselines[x].Name, baselines[x].SampleRate, baselines[x].WaveLength, baselines[x].locked ? "yes" : "no");
                }
                free(baselines);
            } else if(!strcmp(arg, "observation")) {
                int type = 0;
                char *t = strtok(value, "_");
                bool nodelay = false;
                if(!strcmp(t, "synthesis")) {
                    type |= APERTURE_SYNTHESIS;
                } else if(!strcmp(t, "movingbase")) {
                    type &= ~APERTURE_SYNTHESIS;
                } else {
                    return;
                }
                t = strtok(NULL, "_");
                if(!strcmp(t, "nodelay")) {
                    nodelay = true;
                } else if(!strcmp(t, "delay")) {
                    nodelay = false;
                } else {
                    return;
                }
                t = strtok(NULL, "_");
                if(!strcmp(t, "idft")) {
                    type |= UV_IDFT;
                } else if(!strcmp(t, "raw")) {
                } else if(!strcmp(t, "coverage")) {
                    type |= UV_COVERAGE;
                } else {
                    return;
                }
                dsp_stream_p plot = GetPlot(w, h, type, nodelay);
                if (plot != NULL) {
                    if((type & UV_IDFT) != 0) {
                        dsp_stream_p idft = vlbi_get_ifft_estimate(plot);
                        dsp_stream_free_buffer(plot);
                        dsp_stream_free(plot);
                        plot = idft;
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
                AddContext(value);
            }
            else if(!strcmp(arg, "node")) {
                char name[32], file[150], date[64];
                double lat, lon, el;
                int geo = 2;
                char* k = strtok(value, ",");
                strcpy(name, k);
                k = strtok(NULL, ",");
                if(!strcmp(k, "geo"))
                    geo = 1;
                else if(!strcmp(k, "xyz"));
                else {
                    geo = 0;
                    return;
                }
                k = strtok(NULL, ",");
                lat = (double)atof(k);
                k = strtok(NULL, ",");
                lon = (double)atof(k);
                k = strtok(NULL, ",");
                el = (double)atof(k);
                k = strtok(NULL, ",");
                int len = strlen (k);
                char* base64 = (char*)malloc(len);
                char* buf = (char*)malloc(len*3/4+4);
                strcpy(base64, k);
                len = from64tobits_fast(buf, base64, len);
                k = strtok(NULL, ",");
                strcpy(date, k);
                dsp_location location;
                location.geographic.lat = lat;
                location.geographic.lon = lon;
                location.geographic.el = el;
                if(len > 0 && geo > 0) {
                    AddNode(name, &location, buf, len, vlbi_time_string_to_utc(date), geo == 1);
                }
            }
        }
        else if(!strcmp(cmd, "del")) {
            if(!strcmp(arg, "node")) {
                DelNode(value);
            }
            else if(!strcmp(arg, "context")) {
                DelContext(value);
            }
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
    char *cmd, *arg, *value, opt;
    FILE *input = stdin;
    while ((opt = getopt(argc, argv, "t:f:")) != -1) {
        switch (opt) {
        case 't':
            vlbi_max_threads((int)atof(optarg));
            break;
        case 'f':
            input = fopen (optarg, "rb+");
            break;
        default:
            fprintf(stderr, "Usage: %s [-t max_threads] [-f obs_file]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGSTOP, sighandler);
    signal(SIGQUIT, sighandler);
    if(client->Init(argc, argv)) {
        while (is_running) {
            if(feof(input))
                break;
            client->Parse(input);
        }
    }
    return EXIT_SUCCESS;
}
