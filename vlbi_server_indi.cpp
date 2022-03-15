#include "vlbi_server_indi.h"

INDIServer::INDIServer()
    : INDI::BaseClient()
    , VLBI::Server::Server()
{
}

int INDIServer::Init(int argc, char** argv)
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
        perr("usage: %s hostname port\n", argv[0]);
        return 0;
    }
    return 1;
}

INDIServer::~INDIServer()
{
    disconnectServer();
}

void INDIServer::SetCapture(double seconds)
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

void INDIServer::AbortCapture()
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

void INDIServer::SetExposure(double seconds)
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

void INDIServer::AbortExposure()
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

void INDIServer::SetFreq(double centerfrequency)
{
    if(!isServerConnected())
        return;
    VLBI::Server::SetFreq(centerfrequency);
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_FREQUENCY", centerfrequency);
    }
}

void INDIServer::SetSampleRate(double samplingfrequency)
{
    if(!isServerConnected())
        return;
    VLBI::Server::SetSampleRate(samplingfrequency);
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_SAMPLERATE", samplingfrequency);
    }
}

void INDIServer::SetBadwidth(double bandwidth)
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

void INDIServer::SetGain(double gain)
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

void INDIServer::SetBps(int Bps)
{
    if(!isServerConnected())
        return;
    VLBI::Server::SetBps(Bps);
    std::vector<INDI::BaseDevice*> devices;
    getDevices(devices, INDI::BaseDevice::SPECTROGRAPH_INTERFACE);
    for(INDI::BaseDevice* dev : devices)
    {
        dev->setMediator(this);
        this->sendNewNumber(dev->getDeviceName(), "RECEIVER_SETTINGS", "RECEIVER_BITSPERSAMPLE", Bps);
    }
}

void INDIServer::GoTo(double Ra, double Dec)
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

void INDIServer::Connect()
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

void INDIServer::Disconnect()
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

void INDIServer::Tracking(bool on)
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

void INDIServer::Park()
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

void INDIServer::Unpark()
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

void INDIServer::newDevice(INDI::BaseDevice *dev)
{
    INDI_UNUSED(dev);
}

void INDIServer::removeDevice(INDI::BaseDevice *dp)
{
    INDI_UNUSED(dp);
}

void INDIServer::newProperty(INDI::Property *property)
{
    if(!isServerConnected())
        return;
    this->setBLOBMode(B_ONLY, property->getDeviceName());
    INDI_UNUSED(property);
}

void INDIServer::removeProperty(INDI::Property *property)
{
    INDI_UNUSED(property);
}

void INDIServer::newBLOB(IBLOB *bp)
{
    if(!isServerConnected())
        return;
    if(!strcmp(bp->name, "DATA"))
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
    INDI_UNUSED(bp);
}

void INDIServer::newSwitch(ISwitchVectorProperty *svp)
{
    if(!isServerConnected())
        return;
    if(!strcmp(svp->name, "CONNECTION"))
    {
        if(svp->sp[0].s == ISS_ON)
        {
            perr("Device %s connected.\n", svp->device);
        }
        if(svp->sp[0].s == ISS_OFF)
        {
            perr("Device %s disconnected.\n", svp->device);
        }
    }
    INDI_UNUSED(svp);
}

void INDIServer::newNumber(INumberVectorProperty *nvp)
{
    if(!isServerConnected())
        return;
    if(!strcmp(nvp->name, "SENSOR_INTEGRATION"))
    {
        perr("Capture left: %lf.\n", nvp->np[0].value);
        if(nvp->np[0].value < 1.0)
        {
            perr("Capture complete.\n");
        }
    }
    if(!strcmp(nvp->name, "CCD_EXPOSURE"))
    {
        perr("Exposure left: %lf.\n", nvp->np[0].value);
        if(nvp->np[0].value < 1.0)
        {
            perr("Exposure complete.\n");
        }
    }
    if(!strcmp(nvp->name, "EQUATORIAL_EOD_COORDS"))
    {
        perr("Current coordinates: RA:%lf DEC:%lf.\n", nvp->np[0].value, nvp->np[1].value);
    }
    INDI_UNUSED(nvp);
}

void INDIServer::newText(ITextVectorProperty *tvp)
{
    INDI_UNUSED(tvp);
}

void INDIServer::newLight(ILightVectorProperty *lvp)
{
    INDI_UNUSED(lvp);
}

void INDIServer::newMessage(INDI::BaseDevice *dp, int messageID)
{
    INDI_UNUSED(dp);
    INDI_UNUSED(messageID);
}

void INDIServer::serverConnected()
{
    perr("Connected to server\n");
}

void INDIServer::serverDisconnected(int exit_code)
{
    perr("Disconnected from server\n");
    INDI_UNUSED(exit_code);
}

void INDIServer::Parse()
{
    if(!isServerConnected())
        return;
    FILE* f = GetInput();
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
            SetRa((double)atof(k));
            k = strtok(NULL, ",");
            SetDec((double)atof(k));
            GoTo(GetRa(), GetDec());
        }
        else if(!strcmp(arg, "frequency"))
        {
            SetFreq((double)atof(value));
        }
        else if(!strcmp(arg, "samplerate"))
        {
            SetSampleRate((double)atof(value));
        }
        else if(!strcmp(arg, "bitspersample"))
        {
            SetBps((double)atof(value));
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
    VLBI::Server::Parse();
}

Server *VLBI::server = (Server*)new INDIServer();
