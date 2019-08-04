#ifndef VLBI_CLIENT_H
#define VLBI_CLIENT_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "vlbi/base64.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <vlbi.h>
#include <vlbi/instancecollection.h>

namespace VLBI {

#define DFT 1
#define GEOCENTRIC_COORDS 2
#define EARTH_TIDE 4

#define moving_base_raw_abs 0
#define moving_base_dft_abs (DFT)
#define moving_base_raw_geo (GEOCENTRIC_COORDS)
#define moving_base_dft_geo (DFT|GEOCENTRIC_COORDS)
#define earth_tide_raw_abs (EARTH_TIDE)
#define earth_tide_dft_abs (EARTH_TIDE|DFT)
#define earth_tide_raw_geo (EARTH_TIDE|GEOCENTRIC_COORDS)
#define earth_tide_dft_geo (EARTH_TIDE|DFT|GEOCENTRIC_COORDS)

class Client
{
public:
    Client();
    virtual ~Client();

    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }
    virtual int Init(int argc, char** argv) { return 0; };
    virtual void Parse(char* cmd, char* arg, char* value);
    void AddNode(char *name, double lat, double lon, double el, void *buf, int len, timespec starttime);
    void DelNode(char *name);
    dsp_stream_p GetPlot(int u, int v, int type);
    void SetFifo(FILE* fifo) { f = fifo; }

    double Ra;
    double Dec;
    double Freq;
    double SampleRate;
    int Bps;
    double Gain;
    double Bandwidth;
    int w;
    int h;
private:
    FILE *f;
    vlbi_context context;
    dsp_stream_p model;
    dsp_stream_p uv;
    dsp_stream_p fft;
    InstanceCollection *contexts;
};
};

#endif //VLBI_CLIENT_H
