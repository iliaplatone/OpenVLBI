#include "vlbi_client_indi.h"

INDIClient::INDIClient()
    : INDI::BaseClient()
    , VLBI::Client::Client()
{
}

int INDIClient::Init(int argc, char** argv)
{
    if(argc > 1)
    {
        Address = argv[1];
        Port = atoi(argv[2]);
        setServer(Address, Port);
        return connectServer();
    }
    else
    {
        fprintf(stderr, "usage: %s hostname port\n", argv[0]);
        return 0;
    }
    return 1;
}

INDIClient::~INDIClient()
{
    disconnectServer();
}

void INDIClient::SetCapture(double seconds)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "SENSOR_INTEGRATION", "SENSOR_INTEGRATION_VALUE", seconds);
    }
}

void INDIClient::AbortCapture()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "SENSOR_ABORT_INTEGRATION", "ABORT");
    }
}

void INDIClient::SetExposure(double seconds)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::CCD_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", seconds);
    }
}

void INDIClient::AbortExposure()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::CCD_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "CCD_ABORT_EXPOSURE", "ABORT");
    }
}

void INDIClient::SetFrequency(double centerfrequency)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_FREQUENCY", centerfrequency);
    }
}

void INDIClient::SetSampleRate(double samplingfrequency)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_SAMPLERATE", samplingfrequency);
    }
}

void INDIClient::SetBadwidth(double bandwidth)
{
    if(!isServerConnected())
        return;
    Bandwidth = bandwidth;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_BANDWIDTH", bandwidth);
    }
}

void INDIClient::SetGain(double gain)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_GAIN", gain);
    }
}

void INDIClient::SetBps(int Bps)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_BITSPERSAMPLE", Bps);
    }
}

void INDIClient::GoTo(double Ra, double Dec)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "EQUATORIAL_EOD_COORD", "RA", Ra);
        this->sendNewNumber(dev->getDeviceName(), "EQUATORIAL_EOD_COORD", "DEC", Dec);
    }
}

void INDIClient::Connect()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE |
               INDI::BaseDevice::SPECTROGRAPH_INTERFACE | INDI::BaseDevice::CCD_INTERFACE | INDI::BaseDevice::GUIDER_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "CONNECTION", "CONNECT");
    }
}

void INDIClient::Disconnect()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE |
               INDI::BaseDevice::SPECTROGRAPH_INTERFACE | INDI::BaseDevice::CCD_INTERFACE | INDI::BaseDevice::GUIDER_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewSwitch(dev->getDeviceName(), "CONNECTION", "DISCONNECT");
    }
}

void INDIClient::Tracking(bool on)
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        if(dev->getDriverInterface() & (INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE))
        {
            if(on)
                this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_TRACK_STATE", "TRACK_ON");
            else
                this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_TRACK_STATE", "TRACK_OFF");
        }
    }
}

void INDIClient::Park()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        if(dev->getDriverInterface() & (INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE))
            this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_PARK", "PARK");
    }
}

void INDIClient::Unpark()
{
    if(!isServerConnected())
        return;
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        if(dev->getDriverInterface() & (INDI::BaseDevice::TELESCOPE_INTERFACE | INDI::BaseDevice::DOME_INTERFACE))
            this->sendNewSwitch(dev->getDeviceName(), "TELESCOPE_PARK", "UNPARK");
    }
}

void INDIClient::newDevice(INDI::BaseDevice *dev)
{
    INDI_UNUSED(dev);
}

void INDIClient::removeDevice(INDI::BaseDevice *dp)
{
    INDI_UNUSED(dp);
}

void INDIClient::newProperty(INDI::Property *property)
{
    if(!isServerConnected())
        return;
    this->setBLOBMode(B_ONLY, property->getDeviceName());
    INDI_UNUSED(property);
}

void INDIClient::removeProperty(INDI::Property *property)
{
    INDI_UNUSED(property);
}

void INDIClient::newBLOB(IBLOB *bp)
{
    if(!isServerConnected())
        return;
    if(!strcmp(bp->name, "DATA") || !strcmp(bp->name, "CCD1"))
    {
        if(GetContext() != NULL)
        {
            std::string nodename = "";
            nodename.append(bp->bvp->device);
            nodename.append("_");
            nodename.append(bp->name);
            nodename.append("_");
            nodename.append(bp->bvp->timestamp);
            AddNode((char*)nodename.c_str(), (char*)bp->blob);
        }
        else
        {
            INDI_UNUSED(bp);
        }
    }
}

void INDIClient::newSwitch(ISwitchVectorProperty *svp)
{
    if(!isServerConnected())
        return;
    if(!strcmp(svp->name, "CONNECTION"))
    {
        if(svp->sp[0].s == ISS_ON)
        {
            fprintf(stderr, "Device %s connected.\n", svp->device);
        }
        if(svp->sp[0].s == ISS_OFF)
        {
            fprintf(stderr, "Device %s disconnected.\n", svp->device);
        }
    }
}

void INDIClient::newNumber(INumberVectorProperty *nvp)
{
    if(!isServerConnected())
        return;
    if(!strcmp(nvp->name, "SENSOR_INTEGRATION"))
    {
        fprintf(stderr, "Capture left: %lf.\n", nvp->np[0].value);
        if(nvp->np[0].value < 1.0)
        {
            fprintf(stderr, "Capture complete.\n");
        }
    }
    if(!strcmp(nvp->name, "CCD_EXPOSURE"))
    {
        fprintf(stderr, "Exposure left: %lf.\n", nvp->np[0].value);
        if(nvp->np[0].value < 1.0)
        {
            fprintf(stderr, "Exposure complete.\n");
        }
    }
    if(!strcmp(nvp->name, "EQUATORIAL_EOD_COORDS"))
    {
        fprintf(stderr, "Current coordinates: RA:%lf DEC:%lf.\n", nvp->np[0].value, nvp->np[1].value);
    }
}

void INDIClient::newText(ITextVectorProperty *tvp)
{
    INDI_UNUSED(tvp);
}

void INDIClient::newLight(ILightVectorProperty *lvp)
{
    INDI_UNUSED(lvp);
}

void INDIClient::newMessage(INDI::BaseDevice *dp, int messageID)
{
    INDI_UNUSED(dp);
    INDI_UNUSED(messageID);
}

void INDIClient::serverConnected()
{
    fprintf(stderr, "Connected to server\n");
}

void INDIClient::serverDisconnected(int exit_code)
{
    fprintf(stderr, "Disconnected from server\n");
    INDI_UNUSED(exit_code);
}

void INDIClient::Parse()
{
    if(!isServerConnected())
        return;
    FILE* f = input;
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
    str[len - 2] = 0;
    cmd = strtok(str, " ");
    if (cmd == nullptr)
        return;
    arg = strtok(NULL, " ");
    if (arg == nullptr)
        return;
    value = strtok(NULL, " ");
    if(value == nullptr)
        return;
    if(!strcmp(cmd, "set"))
    {
        if(!strcmp(arg, "connection"))
        {
            if(!strcmp(value, "on"))
            {
                Connect();
            }
            if(!strcmp(value, "off"))
            {
                Disconnect();
            }
        }
        else if(!strcmp(arg, "tracking"))
        {
            if(!strcmp(value, "on"))
            {
                Tracking(true);
            }
            if(!strcmp(value, "off"))
            {
                Tracking(false);
            }
        }
        else if(!strcmp(arg, "parking"))
        {
            if(!strcmp(value, "on"))
            {
                Park();
            }
            if(!strcmp(value, "off"))
            {
                Unpark();
            }
        }
        else if(!strcmp(arg, "goto"))
        {
            char* k = strtok(value, ",");
            Ra = (double)atof(k);
            k = strtok(NULL, ",");
            Dec = (double)atof(k);
            GoTo(Ra, Dec);
        }
        else if(!strcmp(arg, "frequency"))
        {
            Freq = (double)atof(value);
            SetFrequency(Freq);
        }
        else if(!strcmp(arg, "samplerate"))
        {
            SampleRate = (double)atof(value);
            SetSampleRate(SampleRate);
        }
        else if(!strcmp(arg, "bitspersample"))
        {
            Bps = (double)atof(value);
            SetBps(Bps);
        }
        else if(!strcmp(arg, "gain"))
        {
            Gain = (double)atof(value);
            SetGain(Gain);
        }
        else if(!strcmp(arg, "bandwidth"))
        {
            Bandwidth = (double)atof(value);
            SetBadwidth(Bandwidth);
        }
        else if(!strcmp(arg, "capture"))
        {
            duration = (double)atof(value);
            SetCapture(duration);
        }
        else if(!strcmp(arg, "exposure"))
        {
            duration = (double)atof(value);
            SetExposure(duration);
        }
        else if(!strcmp(arg, "abort"))
        {
            if(!strcmp(value, "capture"))
                AbortCapture();
            if(!strcmp(value, "exposure"))
                AbortExposure();
        }
    }
    VLBI::Client::Parse();
}

INDIClient* client = new INDIClient();
