#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_client_indi.h"

VLBIClient::VLBIClient(char *address, int port)
	: INDI::BaseClient()
{
    Address = (char*)malloc(strlen(address));
    Port = port;
    strcpy(Address, address);
    setServer(Address, Port);
    connectServer();
}

VLBIClient::~VLBIClient()
{
    disconnectServer();
    if(context != NULL) {
        vlbi_exit(context);
    }
}

void VLBIClient::SetCapture(double seconds)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "DETECTOR_CAPTURE", "DETECTOR_CAPTURE_VALUE", seconds);
    }
}

void VLBIClient::AbortCapture()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "ABORT", "DETECTOR_ABORT_CAPTURE");
    }
}

void VLBIClient::SetExposure(double seconds)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", seconds);
    }
}

void VLBIClient::AbortExposure()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::CCD_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "ABORT", "CCD_ABORT_EXPOSURE");
    }
}

void VLBIClient::SetFrequency(double centerfrequency)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "DETECTOR_SETTINGS", "DETECTOR_FREQUENCY", centerfrequency);
    }
}

void VLBIClient::SetSampleRate(double samplingfrequency)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "DETECTOR_SETTINGS", "DETECTOR_SAMPLERATE", samplingfrequency);
    }
}

void VLBIClient::SetBadwidth(double bandwidth)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "DETECTOR_SETTINGS", "DETECTOR_BANDWIDTH", bandwidth);
    }
}

void VLBIClient::SetGain(double gain)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "DETECTOR_SETTINGS", "DETECTOR_GAIN", gain);
    }
}

void VLBIClient::SetBPS(int BPS)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::DETECTOR_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "DETECTOR_SETTINGS", "DETECTOR_BPS", BPS);
    }
}

void VLBIClient::GoTo(double Ra, double Dec)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "EQUATORIAL_EOD_COORD", "RA", Ra);
        this->sendNewNumber(dev->getDeviceName(), "EQUATORIAL_EOD_COORD", "DEC", Dec);
    }
}

void VLBIClient::Connect()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE | INDI::BaseDevice::DETECTOR_INTERFACE | INDI::BaseDevice::CCD_INTERFACE | INDI::BaseDevice::GUIDER_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "CONNECTION", "CONNECT");
    }
}

void VLBIClient::Disconnect()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE | INDI::BaseDevice::DETECTOR_INTERFACE | INDI::BaseDevice::CCD_INTERFACE | INDI::BaseDevice::GUIDER_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "CONNECTION", "DISCONNECT");
    }
}

void VLBIClient::Tracking(bool on)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        if(dev->getDriverInterface() & (INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE)) {
            if(on)
                this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_TRACK_STATE", "TRACK_ON");
            else
                this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_TRACK_STATE", "TRACK_OFF");
        }
    }
}

void VLBIClient::Park()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        if(dev->getDriverInterface() & (INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE))
            this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_PARK", "PARK");
    }
}

void VLBIClient::Unpark()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices) {
        dev->setMediator(this);
        if(dev->getDriverInterface() & (INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE))
            this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_PARK", "UNPARK");
    }
}

void VLBIClient::newDevice(INDI::BaseDevice *dev) {
    INDI_UNUSED(dev);
}

void VLBIClient::removeDevice(INDI::BaseDevice *dp) {
    INDI_UNUSED(dp);
}

void VLBIClient::newProperty(INDI::Property *property) {
    this->setBLOBMode(B_ONLY, property->getDeviceName());
    INDI_UNUSED(property);
}

void VLBIClient::removeProperty(INDI::Property *property) {
    INDI_UNUSED(property);
}

void VLBIClient::newBLOB(IBLOB *bp) {
    if(!strcmp(bp->label, "CONTINUUM")) {
        if(context != NULL) {
            int status = 0;
            while (status == 0) {
                size_t len = (bp->bloblen-4) * 3/4;
                void* buf = malloc(len);
                from64tobits_fast((char*)(buf), (char*)bp->blob, bp->bloblen);
                dsp_stream_p node = dsp_stream_new();
                fitsfile* f;
                fits_create_memfile(&f, &buf, &len, 1, NULL, &status);
                char starttime[32], ra[32], dec[32], lat[32], lon[32], el[32];
                int Y, M, D, H, m;
                double s;
                long long offset = f->Fptr->datastart;
                fits_read_key(f, 0, (char*)"OBJCTRA", ra, (char*)"Target Ra", &status);
                fits_read_key(f, 0, (char*)"OBJCTDEC", dec, (char*)"Target Dec", &status);
                fits_read_key(f, 0, (char*)"LATITUDE", lat, (char*)"Node Latutude", &status);
                fits_read_key(f, 0, (char*)"LONGITUDE", lon, (char*)"Node Longitude", &status);
                fits_read_key(f, 0, (char*)"ELEVATION", el, (char*)"Node Elevation", &status);
                fits_read_key(f, 0, (char*)"DATE-OBS", starttime, (char*)"Node Elevation", &status);
                fits_str2time(starttime, &Y, &M, &D, &H, &m, &s, &status);
                node->location[0] = atol(lat);
                node->location[1] = atol(lon);
                node->location[2] = atol(el);
                node->location[2] = vlbi_calc_elevation_coarse(node->location[2], node->location[0]);
                node->target[0] = atol(ra);
                node->target[1] = atol(dec);
                dsp_stream_add_dim(node, len-offset);
                dsp_stream_set_buffer(node, (void*)((char*)buf+offset), node->len);
                node->starttimeutc = vlbi_time_mktimespec(Y, M, D, H, m, floor(s), (s - floor(s))*1000000000.0);
                vlbi_add_stream(context, node);
                return;
            }
            char errstr[120];
            fits_get_errstatus(status, errstr);
            fprintf(stdout, "error adding stream: %s.\n", errstr);
        } else {
            fprintf(stdout, "context null.\n");
            INDI_UNUSED(bp);
        }
    }
}

void VLBIClient::newSwitch(ISwitchVectorProperty *svp) {
    if(!strcmp(svp->name, "CONNECTION")) {
        if(svp->sp[0].s == ISS_ON) {
            fprintf(stdout, "Device %s connected.\n", svp->device);
        }
        if(svp->sp[0].s == ISS_OFF) {
            fprintf(stdout, "Device %s disconnected.\n", svp->device);
        }
    }
}

void VLBIClient::newNumber(INumberVectorProperty *nvp) {
    if(!strcmp(nvp->name, "DETECTOR_CAPTURE")) {
        fprintf(stdout, "Capture left: %lf.\n", nvp->np[0].value);
        if(nvp->np[0].value < 1.0) {
            fprintf(stdout, "Capture complete.\n");
        }
    }
    if(!strcmp(nvp->name, "DETECTOR_EXPOSURE")) {
        fprintf(stdout, "Exposure left: %lf.\n", nvp->np[0].value);
        if(nvp->np[0].value < 1.0) {
            fprintf(stdout, "Exposure complete.\n");
        }
    }
    if(!strcmp(nvp->name, "EQUATORIAL_EOD_COORDS")) {
        fprintf(stdout, "Current coordinates: RA:%lf DEC:%lf.\n", nvp->np[0].value, nvp->np[1].value);
    }
}

void VLBIClient::newText(ITextVectorProperty *tvp) {
    INDI_UNUSED(tvp);
}

void VLBIClient::newLight(ILightVectorProperty *lvp) {
    INDI_UNUSED(lvp);
}

void VLBIClient::newMessage(INDI::BaseDevice *dp, int messageID) {
    INDI_UNUSED(dp);
    INDI_UNUSED(messageID);
}

void VLBIClient::serverConnected() {
    fprintf(stdout, "Connected to server\n");
}

void VLBIClient::serverDisconnected(int exit_code) {
    fprintf(stdout, "Disconnected from server\n");
    INDI_UNUSED(exit_code);
}

VLBIClient *client;
int is_running = 1;

void sighandler(int signum)
{
    signal(signum, SIG_IGN);
    client->~VLBIClient();
    signal(signum, sighandler);
    exit(0);
}

static double* correlation_func(double d1, double d2)
{
    static double res = 0;
    res = d1 * d2;
    return &res;
}

#define TXT_LEN
typedef struct _vlbi_context {
    vlbi_context ctx;
    char* name;
} *_vlbi_context_p, _vlbi_context_t;

int main(int argc, char** argv)
{
    double Ra,Dec,Freq,SampleRate,BPS,Gain,Bandwidth;
    FILE* ifile = stdin;
    if(argc > 1) {
        client = new VLBIClient(argv[1], strtol(argv[2], NULL, 10));
        if(argc > 2) {
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
                uv = vlbi_get_uv_plot_earth_tide(client->GetContext(), (vlbi_func2_t)correlation_func, 0, w, h, coords, Freq, SampleRate);
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
