#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fitsio.h>
#include <base64.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vlbi.h>


class VLBIClient
{
public:
    VLBIClient();
    ~VLBIClient();
    
    virtual void SetCapture(double seconds);
    virtual void AbortCapture();
    virtual void SetExposure(double seconds);
    virtual void AbortExposure();
    virtual void SetFrequency(double centerfrequency);
    virtual void SetSampleRate(double samplingfrequency);
    virtual void SetBadwidth(double bandwidth);
    virtual void SetGain(double gain);
    virtual void SetBPS(int BPS);
    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }

private:
    vlbi_context context;
};
