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
#include <dsp.h>
#include <vlbi/instancecollection.h>

namespace VLBI {

#define APERTURE_SYNTHESIS 1
#define UV_COVERAGE 2
#define UV_IDFT 4

class Client
{
public:
    Client();
    virtual ~Client();

    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }
    virtual int Init(int argc, char** argv) { return 0; };
    virtual void Parse(char* cmd, char* arg, char* value);
    void AddNode(char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo);
    void DelNode(char *name);
    dsp_stream_p GetPlot(int u, int v, int type, bool nodelay = false);
    void SetFifo(FILE* fifo) { f = fifo; }

    double Ra;
    double Dec;
    double Freq;
    double SampleRate;
    int Bps;
    double Gain;
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
