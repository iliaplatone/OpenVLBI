#include <cstdio>
#include <cstdlib>
#include <cstring>
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

    inline void SetContext(vlbi_context ctx) { context = ctx; }
    inline vlbi_context GetContext() { return context; }
    virtual int Init(int argc, char** argv);
    virtual void Parse(char* cmd, char* arg, char* value);
    void AddNode(double lat, double lon, double el, double *buf, double len, timespec starttime);

    double Ra;
    double Dec;
    double Freq;
    double SampleRate;
    double BPS;
    double Gain;
    double Bandwidth;
    int w;
    int h;
private:
    vlbi_context context;
};
