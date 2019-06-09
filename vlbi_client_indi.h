#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <indiapi.h>
#include <indicom.h>
#include <defaultdevice.h>
#include <baseclient.h>
#include <indiccd.h>
#include <indidetector.h>
#include <inditelescope.h>
#include <baseclient.h>
#include <fitsio.h>
#include <base64.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vlbi.h>


class VLBIClient : private INDI::BaseClient
{
public:
    VLBIClient(char *address, int port);
    ~VLBIClient();

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
    void serverConnected();
    void serverDisconnected(int exit_code);
    void Connect();
    void Disconnect();
    bool StartServer();
    bool StopServer();
    void SetCapture(double seconds);
    void AbortCapture();
    void SetFrequency(double centerfrequency);
    void SetSampleRate(double samplingfrequency);
    void SetBadwidth(double bandwidth);
    void SetGain(double gain);
    void SetBPS(int BPS);
    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }
    void GoTo(double Ra, double Dec);
    void Park();
    void Unpark();
    void Tracking(bool on);

private:
    vlbi_context context;
    char* Address;
    char* Savedir;
    int Port;
};
