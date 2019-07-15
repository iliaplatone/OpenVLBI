#ifndef _VLBI_CLIENT_H
#define _VLBI_CLIENT_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <base64.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fitsio.h>
#include <vlbi.h>

static double* correlation_func(double d1, double d2)
{
    static double res = 0;
    res = d1 * d2;
    return &res;
}

class VLBIClient
{
public:
    VLBIClient() { }
    ~VLBIClient() { }

    virtual inline void SetFrequency(double centerfrequency) { }
    virtual inline void SetSampleRate(double samplerate) { }
    virtual inline void SetBadwidth(double bandwidth) { }
    virtual inline void SetGain(double gain) { }
    virtual inline void SetBPS(int BPS) { }
    virtual inline void Run(char *cmd, char *arg, char *value) { }
    inline void SetTarget(double Ra, double Dec) { ra = Ra; dec = Dec; }
    inline void CreateContext() { context = vlbi_init(); }
    inline void FreeContext() { vlbi_exit(context); }
    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }
    inline unsigned char* Plot(int u, int v, bool dft, bool earth_tide_observation) {
        if(earth_tide_observation)
            plot = vlbi_get_uv_plot_earth_tide(context, correlation_func, 1, u, v, new double[2] { ra, dec }, frequency, samplerate);
        else
            plot = vlbi_get_uv_plot_moving_baseline(context, correlation_func, 1, u, v, new double[2] { ra, dec }, frequency, samplerate);
	if(dft) {
            dsp_stream *plot = vlbi_get_fft_estimate(plot);
	}
        unsigned char *base64 = (unsigned char*)malloc(plot->len * 4 / 3 + 4);
        unsigned char *buf = (unsigned char*)malloc(plot->len);
	dsp_buffer_copy(plot->buf, buf, plot->len);
        to64frombits(buf, base64, plot->len);
        return base64;
    }

private:
    double frequency;
    double samplerate;
    double bandwidth;
    double gain;
    double BPS;
    double ra;
    double dec;
    dsp_stream_p plot;
    vlbi_context context;
};
#endif // _VLBI_CLIENT_H
