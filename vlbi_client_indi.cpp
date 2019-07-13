#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_client_indi.h"

VLBIClient_INDI::VLBIClient_INDI(char *address, int port)
	: INDI::BaseClient()
	, VLBIClient()
{
    Address = (char*)malloc(strlen(address));
    Port = port;
    strcpy(Address, address);
    setServer(Address, Port);
    connectServer();
}

VLBIClient_INDI::~VLBIClient_INDI()
{
    disconnectServer();
    if(GetContext() != NULL) {
        vlbi_exit(GetContext());
    }
}

void VLBIClient_INDI::SetCapture(double seconds)
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

void VLBIClient_INDI::AbortCapture()
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

void VLBIClient_INDI::SetExposure(double seconds)
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

void VLBIClient_INDI::AbortExposure()
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

void VLBIClient_INDI::SetFrequency(double centerfrequency)
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

void VLBIClient_INDI::SetSampleRate(double samplingfrequency)
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

void VLBIClient_INDI::SetBadwidth(double bandwidth)
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

void VLBIClient_INDI::SetGain(double gain)
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

void VLBIClient_INDI::SetBPS(int BPS)
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

void VLBIClient_INDI::GoTo(double Ra, double Dec)
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

void VLBIClient_INDI::Connect()
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

void VLBIClient_INDI::Disconnect()
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

void VLBIClient_INDI::Tracking(bool on)
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

void VLBIClient_INDI::Park()
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

void VLBIClient_INDI::Unpark()
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

void VLBIClient_INDI::newDevice(INDI::BaseDevice *dev) {
    INDI_UNUSED(dev);
}

void VLBIClient_INDI::removeDevice(INDI::BaseDevice *dp) {
    INDI_UNUSED(dp);
}

void VLBIClient_INDI::newProperty(INDI::Property *property) {
    this->setBLOBMode(B_ONLY, property->getDeviceName());
    INDI_UNUSED(property);
}

void VLBIClient_INDI::removeProperty(INDI::Property *property) {
    INDI_UNUSED(property);
}

void VLBIClient_INDI::newBLOB(IBLOB *bp) {
    if(!strcmp(bp->label, "CONTINUUM")) {
        if(GetContext() != NULL) {
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
                vlbi_add_stream(GetContext(), node);
                return;
            }
            char errstr[120];
            fits_get_errstatus(status, errstr);
        } else {
            fprintf(stdout, "Context null.\n");
            INDI_UNUSED(bp);
        }
    }
}

void VLBIClient_INDI::newSwitch(ISwitchVectorProperty *svp) {
    if(!strcmp(svp->name, "CONNECTION")) {
        if(svp->sp[0].s == ISS_ON) {
            fprintf(stdout, "Device %s connected.\n", svp->device);
        }
        if(svp->sp[0].s == ISS_OFF) {
            fprintf(stdout, "Device %s disconnected.\n", svp->device);
        }
    }
}

void VLBIClient_INDI::newNumber(INumberVectorProperty *nvp) {
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

void VLBIClient_INDI::newText(ITextVectorProperty *tvp) {
    INDI_UNUSED(tvp);
}

void VLBIClient_INDI::newLight(ILightVectorProperty *lvp) {
    INDI_UNUSED(lvp);
}

void VLBIClient_INDI::newMessage(INDI::BaseDevice *dp, int messageID) {
    INDI_UNUSED(dp);
    INDI_UNUSED(messageID);
}

void VLBIClient_INDI::serverConnected() {
    fprintf(stdout, "Connected to server\n");
}

void VLBIClient_INDI::serverDisconnected(int exit_code) {
    fprintf(stdout, "Disconnected from server\n");
    INDI_UNUSED(exit_code);
}

void VLBIClient_INDI::Run(char* cmd, char* arg, char* value) {
    if(!strcmp(cmd, "set")) {
        if(!strcmp(arg, "connection")) {
            if(!strcmp(value, "on")) {
                Connect();
            }
            if(!strcmp(value, "off")) {
                Disconnect();
            }
        }
        else if(!strcmp(arg, "target")) {
            GoTo(Ra, Dec);
        }
        else if(!strcmp(arg, "tracking")) {
            if(!strcmp(value, "on")) {
                Tracking(true);
            }
            if(!strcmp(value, "off")) {
                Tracking(false);
            }
        }
        else if(!strcmp(arg, "parking")) {
            if(!strcmp(value, "on")) {
               Park();
            }
            if(!strcmp(value, "off")) {
                Unpark();
            }
        }
        else if(!strcmp(arg, "target")) {
            char* ra = strtok(value, ",");
            char* dec = strtok(NULL, ",");
            Ra = (double)atof(ra);
            Dec = (double)atof(dec);
            GoTo(Ra, Dec);
        }
        else if(!strcmp(arg, "capture")) {
            double duration = (double)atof(value);
            SetCapture(duration);
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
    }
    else {
        fprintf(stderr, "commands: \nadd\n\tcontext name\n\tset\n\tconnection [on|off]\n\tGetContext() name\n\tparking [on|off]\n\ttracking [on|off]\n\ttarget ra,dec\n\tfrequency freq\n\tsamplerate freq\n\tbandwidth freq\n\tbitspersample bps\n\tgain value\n\tcapture time\n\tmodel name\nget\n\tobservation [fft|mdl|raw]");
    }
}
