#ifndef VLBI_SERVER_H
#define VLBI_SERVER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <vlbi.h>
#include <dsp.h>

namespace VLBI
{

#define APERTURE_SYNTHESIS  (1<<0)
#define UV_COVERAGE         (1<<1)

class Server
{
    public:
        Server(void);
        virtual ~Server(void);

        inline char* CurrentContext(void)
        {
            return context;
        }
        virtual int Init(int argc, char** argv)
        {
            (void)argc;
            (void)argv;
            return 1;
        }
        virtual void Parse(void);
        void AddNode(char *name, char *b64);
        void AddNode(char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo);
        void DelNode(char *name);
        void Plot(char *name, int u, int v, int type, bool nodelay = false);
        void Idft(char *name, char *magnitude, char *phase);
        void Dft(char *model, char *magnitude, char *phase);
        void Mask(char *name, char *model, char *mask);
        void Shift(char *name);
        dsp_stream_p GetModel(char *name);
        char* GetModel(char *name, char *format);
        int GetModels(char** names);
        void AddModel(char* name, char *format, char *b64);
        void DelModel(char* name);
        void SetInput(FILE* in)
        {
            input = in;
        }
        void SetOutput(FILE* out)
        {
            output = out;
        }
        inline FILE* GetInput()
        {
            return input;
        }
        inline FILE* GetOutput()
        {
            return output;
        }

        inline virtual void SetRa(double value)
        {
            Ra = value;
        }
        inline virtual void SetDec(double value)
        {
            Dec = value;
        }
        inline virtual void SetFreq(double value)
        {
            Freq = value;
        }
        inline virtual void SetSampleRate(double value)
        {
            SampleRate = value;
        }
        inline virtual void SetBps(int value)
        {
            Bps = value;
        }
        inline virtual void SetWidth(int value)
        {
            w = value;
        }
        inline virtual void SetHeight(int value)
        {
            h = value;
        }
        void AddContext(char *name);
        void DelContext(char *name);
        void SetContext(char *name);
        vlbi_context GetContext(void);

        inline double GetRa(void)
        {
            return Ra;
        }
        inline double GetDec(void)
        {
            return Dec;
        }
        inline double GetFreq(void)
        {
            return Freq;
        }
        inline double GetSampleRate(void)
        {
            return SampleRate;
        }
        inline double GetBps(void)
        {
            return Bps;
        }
        inline double GetWidth(void)
        {
            return w;
        }
        inline double GetHeight(void)
        {
            return h;
        }


    private:
        double Ra;
        double Dec;
        double Freq;
        double SampleRate;
        int Bps;
        int w;
        int h;
        FILE *input, *output;
        char *context;
};
};

#endif //VLBI_SERVER_H
