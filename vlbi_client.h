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
    inline void SetTarget(double Ra, double Dec) { ra = Ra; dec = Dec; }
    inline void CreateContext() { context = vlbi_init(); }
    inline void FreeContext() { vlbi_exit(context); }
    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context* GetContext() { return context; }
    inline char* GetDFT(int u, int v, bool earth_tide_observation) {
        if(earth_tide_observation)
            plot = vlbi_get_uv_plot_earth_tide(context, correlation_delegate, earth_tide_observation, u, v, new double[] { ra, dec }, frequency, samplerate);
        else
            plot = vlbi_get_uv_plot_moving_baseline(context, correlation_delegate, earth_tide_observation, u, v, new double[] { ra, dec }, frequency, samplerate);
        dsp_stream *fft = vlbi_get_fft_estimation(plot);
        char *base64 = (char*)malloc(fft->len * 4 / 3 + 4);
        to64frombits(fft->buf, base64, fft->len);
        return base64;
    }
    inline char* GetRAW(int u, int v, bool earth_tide_observation) { 
        if(earth_tide_observation)
            plot = vlbi_get_uv_plot_earth_tide(context, correlation_delegate, earth_tide_observation, u, v, new double[] { ra, dec }, frequency, samplerate);
        else
            plot = vlbi_get_uv_plot_moving_baseline(context, correlation_delegate, earth_tide_observation, u, v, new double[] { ra, dec }, frequency, samplerate);
        char *base64 = (char*)malloc(plot->len * 4 / 3 + 4);
        to64frombits(plot->buf, base64, plot->len);
        return base64;
    }

private:
    double frequency;
    double samplingfrequency;
    double bandwidth;
    double gain;
    double BPS;
    double ra;
    double dec;
    dsp_stream_p *plot;
    vlbi_context context;
};
#endif // _VLBI_CLIENT_H
