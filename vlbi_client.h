#ifndef _VLBI_CLIENT_H
#define _VLBI_CLIENT_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fitsio.h>
#include <base64.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vlbi.h>

double correlation_delegate(double value1, double value2)
{
    return value1 * value2;
}

class VLBIClient
{
public:
    VLBIClient();
    ~VLBIClient();
    
    virtual void SetFrequency(double centerfrequency);
    virtual void SetSampleRate(double samplingfrequency);
    virtual void SetBadwidth(double bandwidth);
    virtual void SetGain(double gain);
    virtual void SetBPS(int BPS);
    virtual void SetTarget(double ra, double dec);
    inline void CreateContext() { context = vlbi_init(); }
    inline void FreeContext() { vlbi_exit(context); }
    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context* GetContext() { return context; }
    inline char* GetDFT(int u, int v, bool earth_tide_observation) { return vlbi_get_plot_dft(context, correlation_delegate, earth_tide_observation, u, v, new double[] { ra, dec }, frequency, samplerate); }
    inline char* GetRAW(int u, int v) { return return vlbi_get_plot_raw(context, correlation_delegate, earth_tide_observation, u, v, new double[] { ra, dec }, frequency, samplerate); }

private:
    double frequency;
    double samplingfrequency;
    double bandwidth;
    double gain;
    double BPS;
    double ra;
    double dec;
    vlbi_context context;
};
#endif // _VLBI_CLIENT_H
