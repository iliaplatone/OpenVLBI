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

namespace VLBI
{

#define APERTURE_SYNTHESIS 1
#define UV_COVERAGE 2

class Client
{
    public:
        Client();
        virtual ~Client();

        inline char* CurrentContext()
        {
            return context;
        }
        inline void AddContext(char* name)
        {
            if(!contexts->ContainsKey(name)) contexts->Add(vlbi_init(), name);
        }
        inline void SetContext(char* name)
        {
            if(contexts->ContainsKey(name))
            {
                context = (char*)realloc(context, strlen(name));
                strcpy(context, name);
            }
        }
        inline vlbi_context GetContext()
        {
            if(contexts->ContainsKey(context)) return contexts->Get(context);
            return nullptr;
        }
        inline void DelContext(char* name)
        {
            if(contexts->ContainsKey(name))
            {
                vlbi_context ctx = contexts->Get(name);
                contexts->Remove(name);
                vlbi_exit(ctx);
            }
        }
        virtual int Init(int argc, char** argv)
        {
            (void)argc;
            (void)argv;
            return 1;
        }
        virtual void Parse();
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
        void SetFifo(FILE* fifo)
        {
            output = fifo;
        }

        double Ra;
        double Dec;
        double Freq;
        double SampleRate;
        int Bps;
        int w;
        int h;
        FILE *input, *output;

    private:
        char *context;
        InstanceCollection *contexts;
};
};

#endif //VLBI_CLIENT_H
