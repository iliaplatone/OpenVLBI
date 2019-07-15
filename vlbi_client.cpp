#include "vlbi_client.h"
#ifdef INDI_CLIENT
#include "vlbi_client_indi.h"
#endif

VLBIClient *client;
int is_running = 1;

typedef struct _vlbi_context {
    vlbi_context ctx;
    char* name;
} *_vlbi_context_p, _vlbi_context_t;

void sighandler(int signum)
{
    signal(signum, SIG_IGN);
    client->~VLBIClient();
    signal(signum, sighandler);
    exit(0);
}

#define TXT_LEN

int main(int argc, char** argv)
{
    double Ra,Dec,Freq,SampleRate,BPS,Gain,Bandwidth;
    FILE* ifile = stdin;
    if(argc > 1) {
        ifile = fopen(argv[1], "r+");
        if(argc > 2) {
#ifdef INDI_CLIENT
            client = new VLBIClient_INDI(argv[2], strtol(argv[3], NULL, 10));
#endif
        }
    } else {
        fprintf(stderr, "usage: %s hostname port [command_file]\n", argv[0]);
        return -22;
    }
    Ra = Dec = Freq = SampleRate = BPS = Gain = Bandwidth = 0;
    _vlbi_context_p* contexts = (_vlbi_context_p*)malloc(sizeof(_vlbi_context_p));
    int num_contexts = 0;
    int w = 128, h = 128;
    dsp_stream_p model = dsp_stream_new();

    dsp_stream_p uv = NULL;
    dsp_stream_p fft = NULL;

    char cmd[150], arg[150], value[150];
    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGSTOP, sighandler);
    signal(SIGQUIT, sighandler);
    while (is_running) {
        if(fscanf(ifile, "%s %s %s", cmd, arg, value) != 3)continue;
        if(!strcmp(cmd, "set")) {
            if(!strcmp(arg, "context")) {
                int i = 0;
                while(strncmp(contexts[i]->name, value, strlen(value)) != 0 && i < num_contexts) {
                    i++;
                }
                if(i > 0 && !strcmp(contexts[i]->name, value)) {
                    client->SetContext(contexts[i]->ctx);
                }
            }
            else if(!strcmp(arg, "resolution")) {
                w = (int)strtol(strtok(value, "x"), NULL, 10);
                h = (int)strtol(NULL, NULL, 10);
            }
            else if(!strcmp(arg, "target")) {
                char* ra = strtok(value, ",");
                char* dec = strtok(NULL, ",");
                Ra = (double)atof(ra);
                Dec = (double)atof(dec);
            }
            else if(!strcmp(arg, "frequency")) {
                Freq = (double)atof(value);
                client->SetFrequency((double)atof(value));
            }
            else if(!strcmp(arg, "samplerate")) {
                SampleRate = (double)atof(value);
                client->SetSampleRate((double)atof(value));
            }
            else if(!strcmp(arg, "bitspersample")) {
                BPS = (double)atof(value);
                client->SetBPS((double)atof(value));
            }
            else if(!strcmp(arg, "gain")) {
                Gain = (double)atof(value);
                client->SetGain((double)atof(value));
            }
            else if(!strcmp(arg, "bandwidth")) {
                Bandwidth = (double)atof(value);
                client->SetBadwidth((double)atof(value));
            }
            else if(!strcmp(arg, "model")) {
                dsp_stream_free(model);
                model = dsp_stream_new();
                fitsfile* f;
                int status = 0;
                fits_open_image(&f, value, 1, &status);
                long long offset = f->Fptr->datastart;
                fits_close_file(f, &status);
                FILE *file = fopen(value, "r");
                fseek(file, 0, SEEK_END);
                unsigned int len = ftell(file)-offset;
                dsp_stream_add_dim(model, len);
                fseek(file, offset, SEEK_SET);
                len = fread(model->buf, 1, len, file);
                fclose(file);
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
        }
        else if(!strcmp(cmd, "get")) {
            if(!strcmp(arg, "observation")) {
                double coords[] =  { Ra, Dec };
                if(!strcmp(value, "fft")) {
                    unsigned char* base64 = client->Plot(w, h, false, true);
                    fwrite(base64, 1, strlen((char*)base64), stdout);
                    free(base64);
                }
                else if(!strcmp(value, "mdl")) {
                }
                else if(!strcmp(value, "raw")) {
                    unsigned char* base64 = client->Plot(w, h, false, false);
                    fwrite(base64, 1, strlen((char*)base64), stdout);
                    free(base64);
                }
            }
        }
        else {
            if(!client->Run(cmd, arg, value))
                fprintf(stderr, "-ERR");
        }
    }
    return 0;
}

