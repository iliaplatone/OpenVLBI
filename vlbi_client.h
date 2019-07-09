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
    inline void CreateContext() { context = vlbi_init(); }
    inline void FreeContext() { vlbi_exit(context); }
    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context* GetContext() { return context; }
    inline char* GetDFT() { return vlbi_get_plot_dft(response->samplerate, response->frequency, response->bandwidth); }
    inline char* GetRAW() { return response->plot; }

private:
    vlbi_context context;
};
#endif // _VLBI_CLIENT_H
