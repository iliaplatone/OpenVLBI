#ifndef VLBI_SERVER_INDI_H
#define VLBI_SERVER_INDI_H

#include "vlbi_server.h"
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

class INDIServer : private INDI::BaseClient, public VLBI::Server
{
    public:
        INDIServer();
        ~INDIServer() override;

        void newDevice(INDI::BaseDevice *dp) override;
        void removeDevice(INDI::BaseDevice *dp) override;
        void newProperty(INDI::Property *property) override;
        void removeProperty(INDI::Property *property) override;
        void newBLOB(IBLOB *bp) override;
        void newSwitch(ISwitchVectorProperty *svp) override;
        void newNumber(INumberVectorProperty *nvp) override;
        void newText(ITextVectorProperty *tvp) override;
        void newLight(ILightVectorProperty *lvp) override;
        void newMessage(INDI::BaseDevice *dp, int messageID) override;
        void newUniversalMessage(std::string) override { }
        void serverConnected() override;
        void serverDisconnected(int exit_code) override;
        void Connect();
        void Disconnect();
        bool StartServer();
        bool StopServer();
        void SetCapture(double seconds);
        void AbortCapture();
        void SetExposure(double seconds);
        void AbortExposure();
        void SetFreq(double centerfrequency) override;
        void SetSampleRate(double samplingfrequency) override;
        void SetBadwidth(double bandwidth);
        void SetGain(double gain);
        void SetBps(int Bps) override;
        void GoTo(double Ra, double Dec);
        void Park();
        void Unpark();
        void Tracking(bool on);

        int Init(int argc, char** argv) override;
        void Parse() override;

    private:
        double Gain;
        char* Address;
        char* Savedir;
        double Bandwidth;
        double duration;
        int Port;
};
#endif //VLBI_SERVER_INDI_H
