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

namespace VLBI {
typedef struct _vlbi_context {
    vlbi_context ctx;
    char* name;
} *_vlbi_context_p, _vlbi_context_t;

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
    int num_contexts;
    vlbi_context context;
    _vlbi_context_p* contexts;
    dsp_stream_p model;
    dsp_stream_p uv;
    dsp_stream_p fft;
};
};

#endif //VLBI_CLIENT_H
