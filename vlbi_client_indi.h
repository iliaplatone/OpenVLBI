#ifndef VLBI_CLIENT_INDI_H
#define VLBI_CLIENT_INDI_H

#include "vlbi_client.h"
#include <indiapi.h>
#include <indicom.h>
#include <defaultdevice.h>
#include <baseclient.h>
#include <indiccd.h>
#include <indisensorinterface.h>
#include <inditelescope.h>
#include <baseclient.h>
#include <fitsio.h>

using namespace VLBI;

class INDIClient : private INDI::BaseClient, public VLBI::Client
{
public:
    INDIClient();
    ~INDIClient();

    void newDevice(INDI::BaseDevice *dp);
    void removeDevice(INDI::BaseDevice *dp);
    void newProperty(INDI::Property *property);
    void removeProperty(INDI::Property *property);
    void newBLOB(IBLOB *bp);
    void newSwitch(ISwitchVectorProperty *svp);
    void newNumber(INumberVectorProperty *nvp);
    void newText(ITextVectorProperty *tvp);
    void newLight(ILightVectorProperty *lvp);
    void newMessage(INDI::BaseDevice *dp, int messageID);
    void newUniversalMessage(std::string) { }
    void serverConnected();
    void serverDisconnected(int exit_code);
    void Connect();
    void Disconnect();
    bool StartServer();
    bool StopServer();
    void SetCapture(double seconds);
    void AbortCapture();
    void SetExposure(double seconds);
    void AbortExposure();
    void SetFrequency(double centerfrequency);
    void SetSampleRate(double samplingfrequency);
    void SetBadwidth(double bandwidth);
    void SetGain(double gain);
    void SetBps(int Bps);
    void GoTo(double Ra, double Dec);
    void Park();
    void Unpark();
    void Tracking(bool on);

    int Init(int argc, char** argv);
    void Parse(FILE* f);

private:
    double Gain;
    char* Address;
    char* Savedir;
    double Bandwidth;
    double duration;
    int Port;
};
#endif //VLBI_CLIENT_INDI_H
