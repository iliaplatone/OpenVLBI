#include "vlbi_client.h"
#include "vlbi_client_indi.h"

VLBIClient *client;

int main(int argc, char** argv)
{
    double Ra,Dec,Freq,SampleRate,BPS,Gain,Bandwidth;
    FILE* ifile = stdin;
    if(argc > 2) {
        if(!strcmp(argv[0], "indi"))
            client = new VLBIClient_INDI(argv[1], strtol(argv[2], NULL, 10));
        else
            client = new VLBIClient_JSON();
        if(argc > 3) {
            ifile = fopen(argv[3], "r+");
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
        if(fscanf(ifile, "%s %s %s", cmd, arg, value) != 3)break;
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
            else if(!strcmp(arg, "connection")) {
                if(!strcmp(value, "on")) {
                    client->Connect();
                }
                if(!strcmp(value, "off")) {
                    client->Disconnect();
                }
            }
            else if(!strcmp(arg, "tracking")) {
                if(!strcmp(value, "on")) {
                    client->Tracking(true);
                }
                if(!strcmp(value, "off")) {
                    client->Tracking(false);
                }
            }
            else if(!strcmp(arg, "parking")) {
                if(!strcmp(value, "on")) {
                    client->Park();
                }
                if(!strcmp(value, "off")) {
                    client->Unpark();
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
                client->GoTo(Ra, Dec);
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
            else if(!strcmp(arg, "capture")) {
                double duration = (double)atof(value);
                client->SetCapture(duration);
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
                uv = vlbi_get_uv_plot_astro(client->GetContext(), (vlbi_func2_t)correlation_func, 0, w, h, coords, Freq, SampleRate);
                if(!strcmp(value, "fft")) {
                    if (uv != NULL) {
                        dsp_stream_p fft = vlbi_get_fft_estimate(uv);
                        if (fft != NULL) {
                            unsigned int len = fft->len * 4 / 3 + 4;
                            unsigned char* base64 = (unsigned char*)malloc(len);
                            to64frombits(base64, (unsigned char*)fft->buf, fft->len);
                            if(len<fwrite(base64, 1, len, stdout))continue;
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
        else {
            fprintf(stderr, "commands: \nadd\n\tcontext name\n\tset\n\tconnection [on|off]\n\tcontext name\n\tparking [on|off]\n\ttracking [on|off]\n\ttarget ra,dec\n\tfrequency freq\n\tsamplerate freq\n\tbandwidth freq\n\tbitspersample bps\n\tgain value\n\tcapture time\n\tmodel name\nget\n\tobservation [fft|mdl|raw]");
        }
    }
    return 0;
}
