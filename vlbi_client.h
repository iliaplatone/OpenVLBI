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
#define GEODETIC_COORDS 2
#define EARTH_TIDE 4

enum plot_type_t {
	moving_base_raw_abs,
	moving_base_dft_abs,
	moving_base_raw_geo=0,
	moving_base_dft_geo,
	earth_tide_raw_abs,
	earth_tide_dft_abs,
	earth_tide_raw_geo,
	earth_tide_dft_geo,
};

class Client
{
public:
    Client();
    virtual ~Client();

    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }
    virtual int Init(int argc, char** argv) { return 0; };
    virtual void Parse(char* cmd, char* arg, char* value);
    void AddNode(double lat, double lon, double el, double *buf, double len, timespec starttime);
    dsp_stream_p GetPlot(int u, int v, plot_type_t type);

    double Ra;
    double Dec;
    double Freq;
    double SampleRate;
    double Bps;
    double Gain;
    double Bandwidth;
    int w;
    int h;
private:
    vlbi_context context;
    dsp_stream_p model;
    dsp_stream_p uv;
    dsp_stream_p fft;
    InstanceCollection *contexts;
};
};

#endif //VLBI_CLIENT_H
