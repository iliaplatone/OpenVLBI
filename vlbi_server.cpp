/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2023  Ilia Platone
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "vlbi_server.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <vlbi/base64.h>
#include <vlbi/instancecollection.h>
#ifdef _WIN32
#include "getline.c"
#endif

static int is_running = 1;

static InstanceCollection *contexts;
VLBI::Server::Server()
{
    setInput(fdopen(0, "rb"));
    setOutput(fdopen(1, "w+"));
    Ra = 0;
    Dec = 0;
    Freq = 1420000000;
    SampleRate = 100000000;
    Bps = 8;
    w = 128;
    h = 128;
    if(nullptr == tmpdir)
        tmpdir = getenv("TMPDIR");
    if(nullptr == tmpdir)
        tmpdir = getenv("TMP");
    if(nullptr == tmpdir)
        tmpdir = getenv("TEMP");
    if(nullptr == tmpdir)
        tmpdir = (char*)"/tmp";
    context = (char*)malloc(9);
    memcpy(context, "OpenVLBI\0", 9);
    contexts = new InstanceCollection();
}

VLBI::Server::~Server()
{
    if(getContext() != nullptr)
    {
        vlbi_exit(getContext());
    }
}

static double coverage_delegate(double x, double y)
{
    (void)x;
    (void)y;
    return 1.0;
}

static double default_delegate(double x, double y)
{
    return x*y;
}

void VLBI::Server::addNode(const char *name, char *b64)
{
    char filename[128];
    strcpy(filename, tmpdir);
    strcat(filename, "/tmp_nodeXXXXXX");
    int fd = mkstemp(filename);
    if(fd > -1)
    {
        size_t b64len = strlen(b64);
        char* buf = (char*)malloc(b64len * 3 / 4 + 4);
        size_t len = (size_t)from64tobits_fast(buf, b64, (int)b64len);
        ssize_t written = write(fd, buf, len);
        (void)written;
        free(buf);
        close(fd);
        vlbi_add_node_from_fits(getContext(), filename, name, true);
        unlink(filename);
    }
}

void VLBI::Server::addNodes(const char *name, char *b64)
{
    char filename[128];
    strcpy(filename, tmpdir);
    strcat(filename, "/tmp_nodeXXXXXX");
    int fd = mkstemp(filename);
    if(fd > -1)
    {
        size_t b64len = strlen(b64);
        char* buf = (char*)malloc(b64len * 3 / 4 + 4);
        size_t len = (size_t)from64tobits_fast(buf, b64, (int)b64len);
        ssize_t written = write(fd, buf, len);
        (void)written;
        free(buf);
        close(fd);
        vlbi_add_nodes_from_sdfits(getContext(), filename, name, true);
        unlink(filename);
    }
}

void VLBI::Server::addNode(const char *name, dsp_location *locations, void *buf, int bytelen, timespec starttime, bool geo)
{
    dsp_stream_p node = dsp_stream_new();
    int len = bytelen * 8 / abs(Bps);
    dsp_stream_add_dim(node, len);
    dsp_stream_alloc_buffer(node, len);
    switch(Bps)
    {
        case 16:
            dsp_buffer_copy(((unsigned short int*)buf), node->buf, len);
            break;
        case 32:
            dsp_buffer_copy(((unsigned int*)buf), node->buf, len);
            break;
        case 64:
            dsp_buffer_copy(((unsigned long int*)buf), node->buf, len);
            break;
        case -32:
            dsp_buffer_copy(((float*)buf), node->buf, len);
            break;
        case -64:
            dsp_buffer_copy(((double*)buf), node->buf, len);
            break;
        case 8:
            dsp_buffer_copy(((unsigned char*)buf), node->buf, len);
            break;
        default:
            break;
    }
    node->location = locations;
    memcpy(&node->starttimeutc, &starttime, sizeof(timespec));
    vlbi_add_node(getContext(), node, name, geo);
}

void VLBI::Server::delNode(const char *name)
{
    vlbi_del_node(getContext(), name);
}

void VLBI::Server::Plot(const char *name, int flags)
{
    double coords[3] = { Ra, Dec, DBL_MAX };
    if((flags & plot_flags_custom_delegate) == 0) {
        setDelegate((flags & plot_flags_uv_coverage) != 0 ? coverage_delegate : default_delegate);
    }
    vlbi_get_uv_plot(getContext(), name, w, h, coords, Freq, SampleRate, (flags & plot_flags_synced) != 0,
                     (flags & plot_flags_moving_baseline) != 0,
                     getDelegate(), nullptr);
}

void VLBI::Server::Idft(const char *model, const char *magnitude, const char *phase)
{
    vlbi_get_ifft(getContext(), model, magnitude, phase);
}

void VLBI::Server::Dft(const char *model, const char *magnitude, const char *phase)
{
    vlbi_get_fft(getContext(), model, magnitude, phase);
}

void VLBI::Server::Stack(const char *name, const char *model1, const char *model2)
{
    vlbi_stack_models(getContext(), name, model1, model2);
}

void VLBI::Server::Diff(const char *name, const char *model1, const char *model2)
{
    vlbi_diff_models(getContext(), name, model1, model2);
}

void VLBI::Server::Convolve(const char *name, const char *model1, const char *model2)
{
    vlbi_apply_convolution_matrix(getContext(), name, model1, model2);
}

void VLBI::Server::Mask(const char *name, const char *model, const char *mask)
{
    vlbi_apply_mask(getContext(), name, model, mask);
}

void VLBI::Server::Copy(const char *name, const char *model)
{
    vlbi_copy_model(getContext(), name, model);
}

void VLBI::Server::CopyNode(const char *name, const char *node)
{
    vlbi_copy_model(getContext(), name, node);
}

void VLBI::Server::Shift(const char *name)
{
    vlbi_shift(getContext(), name);
}

void VLBI::Server::LowPass(const char *name, const char *node, double freq)
{
    vlbi_filter_lp_node(getContext(), name, node, freq);
}

void VLBI::Server::HighPass(const char *name, const char *node, double freq)
{
    vlbi_filter_hp_node(getContext(), name, node, freq);
}

void VLBI::Server::BandPass(const char *name, const char *node, double lofreq, double hifreq)
{
    vlbi_filter_bp_node(getContext(), name, node, lofreq, hifreq);
}

void VLBI::Server::BandReject(const char *name, const char *node, double lofreq, double hifreq)
{
    vlbi_filter_br_node(getContext(), name, node, lofreq, hifreq);
}

dsp_stream_p VLBI::Server::getModel(const char *name)
{
    return vlbi_get_model(getContext(), name);
}

void VLBI::Server::setCorrelationOrder(int order)
{
    vlbi_set_correlation_order(getContext(), order);
}

char* VLBI::Server::getModel(const char *name, char *format)
{
    if(!vlbi_has_model(getContext(), name)) return nullptr;
    char filename[128];
    int channels = 1;
    int fd = -1;
    ssize_t len = 0;
    ssize_t outlen = 0;
    unsigned char *buf = nullptr;
    unsigned char *b64 = nullptr;
    strcpy(filename, tmpdir);
    strcat(filename, "/tmp_modelXXXXXX");
    fd = mkstemp(filename);
    dsp_stream_p model = vlbi_get_model(getContext(), name);
    dsp_stream_p *stream = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * (size_t)(channels + 1));
    for(int c = 0; c <= channels; c++)
        stream[c] = dsp_stream_copy(model);
    if(fd > -1)
    {
        if(!strcmp(format, "jpeg"))
            dsp_file_write_jpeg_composite(filename, channels, 100, stream);
        if(!strcmp(format, "png"))
            dsp_file_write_png_composite(filename, channels, 9, stream);
        if(!strcmp(format, "fits"))
            dsp_file_write_fits_composite(filename, channels, 16, stream);
        FILE* f = fdopen(fd, "rb+");
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        rewind(f);
        len -= ftell(f);
        buf = (unsigned char*)malloc((size_t)len);
        size_t nread = fread(buf, 1, (size_t)len, f);
        outlen = nread * 4 / 3 + 4;
        b64 = (unsigned char*)malloc((size_t)outlen);
        to64frombits(b64, buf, (int)nread);
        free(buf);
        close(fd);
        unlink(filename);
        return (char*)b64;
    }
    return nullptr;
}

int VLBI::Server::getModels(char** names)
{
    dsp_stream_p* models;
    int len = vlbi_get_models(getContext(), &models);
    if(len > 0)
    {
        names = (char**)malloc(sizeof(char*) * (size_t)len);
        for(int x = 0; x < len; x++)
        {
            names[x] = models[x]->name;
        }
        return len;
    }
    return 0;
}

void VLBI::Server::delModel(const char *name)
{
    vlbi_del_model(getContext(), name);
}

void VLBI::Server::addModel(const char *name, char *format, char *b64)
{
    if(vlbi_has_model(getContext(), name)) return;
    char filename[128];
    int fd = -1;
    size_t b64len = 0;
    char *buf = nullptr;
    strcpy(filename, tmpdir);
    strcat(filename, "/tmp_modelXXXXXX");
    fd = mkstemp(filename);
    b64len = strlen(b64);
    if(b64len > 0)
    {
        if(fd > -1)
        {
            buf = (char*)malloc(b64len * 3 / 4 + 4);
            size_t len = (size_t)from64tobits_fast(buf, b64, (int)b64len);
            ssize_t written = write(fd, buf, len);
            (void)written;
            free(buf);
            close(fd);
            if(!strcmp(format, "fits"))
            {
                vlbi_add_model_from_fits(getContext(), filename, name);
            }
            if(!strcmp(format, "jpeg"))
            {
                vlbi_add_model_from_jpeg(getContext(), filename, name);
            }
            if(!strcmp(format, "png"))
            {
                vlbi_add_model_from_png(getContext(), filename, name);
            }
            unlink(filename);
        }
    }
}

void VLBI::Server::Parse()
{
    FILE* f = input;
    size_t len = 0;
    char *cmd = nullptr;
    char *arg = nullptr;
    char *value = nullptr;
    char *str = (char*)malloc(1);
    while(true) {
        str[len] = 0;
        fread(&str[len], 1, 1, f);
        if(str[len++] == 0) break;
        str = (char*)realloc(str, len + 8);
    }
    if(strlen(str) <= 0)
        return;
    if (len == 0)
        return;
    *strrchr(str, '\n') = 0;
    cmd = strtok(str, " ");
    if (cmd == nullptr)
        return;
    if (!strcmp(cmd, "quit"))
    {
        is_running = false;
        return;
    }
    else
    {
        arg = strtok(nullptr, " ");
        if (arg == nullptr)
            return;
        value = strtok(nullptr, " ");
        if(value == nullptr)
            return;
        if(!strcmp(cmd, "set"))
        {
            if(!strcmp(arg, "context"))
            {
                setContext(value);
            }
            else if(!strcmp(arg, "mask"))
            {
                char *t = strtok(value, ",");
                const char *name = t;
                if(name == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *model = t;
                if(model == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *mask = t;
                if(mask == nullptr)
                {
                    return;
                }
                Mask(name, model, mask);
            }
            else if(!strcmp(arg, "shifted"))
            {
                Shift(value);
            }
            else if(!strcmp(arg, "resolution"))
            {
                char* W = strtok(value, "x");
                char* H = strtok(nullptr, "x");
                w = (int)atof(W);
                h = (int)atof(H);
            }
            else if(!strcmp(arg, "target"))
            {
                char* ra = strtok(value, ",");
                char* dec = strtok(nullptr, ",");
                Ra = (double)atof(ra);
                Dec = (double)atof(dec);
            }
            else if(!strcmp(arg, "frequency"))
            {
                Freq = (double)atof(value);
            }
            else if(!strcmp(arg, "samplerate"))
            {
                SampleRate = (double)atof(value);
            }
            else if(!strcmp(arg, "bitspersample"))
            {
                Bps = (int)atof(value);
            }
            else if(!strcmp(arg, "location"))
            {
                double lat, lon, el;
                char *t = strtok(value, ",");
                lat = (int)atof(t);
                t = strtok(nullptr, ",");
                lon = (int)atof(t);
                t = strtok(nullptr, ",");
                el = (int)atof(t);
                vlbi_set_location(getContext(), lat, lon, el);
            }
        }
        else if(!strcmp(cmd, "get"))
        {
            if(!strcmp(arg, "models"))
            {
                dsp_stream_p* models;
                int n = vlbi_get_models(getContext(), &models);
                for(int x = 0; x < n; x++)
                {
                    fprintf(f, "Model #%d: name:%s width:%d height:%d\n", x,
                            models[x]->name, models[x]->sizes[0], models[x]->sizes[1]);
                }
                free(models);
            }
            if(!strcmp(arg, "nodes"))
            {
                vlbi_node* nodes;
                int n = vlbi_get_nodes(getContext(), &nodes);
                for(int x = 0; x < n; x++)
                {
                    fprintf(f, "Node #%d: name:%s relative?:%s x:%lf y:%lf z:%lf latitude:%lf longitude:%lf elevation:%lf\n", nodes[x].Index,
                            nodes[x].Name, nodes[x].Geo ? "no" : "yes", nodes[x].Location[0], nodes[x].Location[1], nodes[x].Location[2],
                            nodes[x].GeographicLocation[0], nodes[x].GeographicLocation[1], nodes[x].GeographicLocation[2]);
                }
                free(nodes);
            }
            else if(!strcmp(arg, "baselines"))
            {
                vlbi_baseline* baselines;
                int n = vlbi_get_baselines(getContext(), &baselines);
                for(int x = 0; x < n; x++)
                {
                    fprintf(f, "Baseline #%d: name:%s samplerate:%lf wavelength:%lf custom?:%s\n", x, baselines[x].Name,
                            baselines[x].SampleRate, baselines[x].WaveLength, baselines[x].locked ? "yes" : "no");
                }
                free(baselines);
            }
            else if(!strcmp(arg, "model"))
            {
                char *t = strtok(value, ",");
                const char *name = t;
                if(name == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *format = t;
                if(format == nullptr)
                {
                    return;
                }
                char *base64 = getModel(name, format);
                fwrite(base64, 1, strlen(base64), output);
                free(base64);
            }
        }
        else if(!strcmp(cmd, "add"))
        {
            if(!strcmp(arg, "context"))
            {
                addContext(value);
            }
            else if(!strcmp(arg, "node"))
            {
                char name[32], date[64];
                double lat, lon, el;
                int geo = 2;
                char* k = strtok(value, ",");
                strcpy(name, k);
                k = strtok(nullptr, ",");
                if(!strcmp(k, "geo"))
                    geo = 1;
                else if(!strcmp(k, "xyz"));
                else
                {
                    geo = 0;
                    return;
                }
                k = strtok(nullptr, ",");
                lat = (double)atof(k);
                k = strtok(nullptr, ",");
                lon = (double)atof(k);
                k = strtok(nullptr, ",");
                el = (double)atof(k);
                k = strtok(nullptr, ",");
                size_t len = strlen (k);
                char* base64 = (char*)malloc(len);
                char* buf = (char*)malloc(len * 3 / 4 + 4);
                strcpy(base64, k);
                len = (size_t)from64tobits_fast(buf, base64, (int)len);
                k = strtok(nullptr, ",");
                strcpy(date, k);
                dsp_location location;
                location.geographic.lat = lat;
                location.geographic.lon = lon;
                location.geographic.el = el;
                if(len > 0 && geo > 0)
                {
                    addNode(name, &location, buf, (int)len, vlbi_time_string_to_timespec(date), geo == 1);
                }
            }
            else if(!strcmp(arg, "plot"))
            {
                int flags = 0;
                char *t = strtok(value, ",");
                const char *name = t;
                if(name == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                if(!strcmp(t, "synthesis"))
                {
                }
                else if(!strcmp(t, "movingbase"))
                {
                    flags |= plot_flags_moving_baseline;
                }
                else
                {
                    return;
                }
                t = strtok(nullptr, ",");
                if(!strcmp(t, "nodelay"))
                {
                    flags |= plot_flags_synced;
                }
                else if(!strcmp(t, "delay"))
                {
                }
                else
                {
                    return;
                }
                t = strtok(nullptr, ",");
                if(!strcmp(t, "raw"))
                {
                }
                else if(!strcmp(t, "coverage"))
                {
                    flags |= plot_flags_uv_coverage;
                }
                else
                {
                    return;
                }
                Plot(name, flags);
            }
            else if(!strcmp(arg, "idft"))
            {
                char *t = strtok(value, ",");
                char *model = t;
                if(model == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *magnitude = t;
                if(magnitude == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *phase = t;
                if(phase == nullptr)
                {
                    return;
                }
                Idft(model, magnitude, phase);
            }
            else if(!strcmp(arg, "dft"))
            {
                char *t = strtok(value, ",");
                char *model = t;
                if(model == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *magnitude = t;
                if(magnitude == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *phase = t;
                if(phase == nullptr)
                {
                    return;
                }
                Dft(model, magnitude, phase);
            }
            else if(!strcmp(arg, "model"))
            {
                char *t = strtok(value, ",");
                const char *name = t;
                if(name == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *format = t;
                if(format == nullptr)
                {
                    return;
                }
                t = strtok(nullptr, ",");
                char *b64 = t;
                if(b64 == nullptr)
                {
                    return;
                }
                addModel(name, format, b64);
            }
        }
        else if(!strcmp(cmd, "del"))
        {
            if(!strcmp(arg, "node"))
            {
                delNode(value);
            }
            else if(!strcmp(arg, "context"))
            {
                delContext(value);
            }
            else if(!strcmp(arg, "model"))
            {
                delModel(value);
            }
        }
    }
}

void VLBI::Server::addContext(const char *name)
{
    if(!contexts->contains(name)) contexts->add(vlbi_init(), name);
}

void VLBI::Server::setContext(const char *name)
{
    if(contexts->contains(name))
    {
        context = (char*)realloc(context, strlen(name));
        strcpy(context, name);
    }
}

vlbi_context VLBI::Server::getContext()
{
    if(contexts->contains(context)) return contexts->get(context);
    return nullptr;
}

void VLBI::Server::delContext(const char *name)
{
    if(contexts->contains(name))
    {
        vlbi_context ctx = contexts->get(name);
        contexts->remove(name);
        vlbi_exit(ctx);
    }
}

static void sighandler(int signum)
{
    signal(signum, SIG_IGN);
    VLBI::server->~Server();
    signal(signum, sighandler);
    exit(0);
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
    int argc = 0;
    char **argv = {NULL};
#else
int main(int argc, char *argv[])
{
#endif
    int opt;
    dsp_set_app_name(argv[0]);
    dsp_set_stdout(fdopen(2, "w+"));
    dsp_set_stderr(fdopen(2, "w+"));
    while ((opt = getopt(argc, argv, "t:f:o:vh")) != -1)
    {
        switch (opt)
        {
            case 't':
                //vlbi_max_threads((unsigned long)atol(optarg));
                break;
            case 'f':
                //VLBI::server->setInput(fopen (optarg, "rb+"));
                break;
            case 'o':
                //VLBI::server->setOutput(fopen (optarg, "a"));
                break;
            case 'v':
                dsp_set_debug_level(dsp_get_debug_level()+1);
            break;
            case 'h':
            default:
                perr("Usage: %s [-t max_threads] [-f obs_file] [-o obs_file] [-v[v[v]]]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    signal(SIGTERM, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGINT, sighandler);
#ifndef _WIN32
    signal(SIGSTOP, sighandler);
    signal(SIGKILL, sighandler);
    signal(SIGQUIT, sighandler);
#endif
    if(VLBI::server->Init(argc, argv))
    {
        while (is_running)
        {
            if(feof(VLBI::server->getInput()))
                break;
            VLBI::server->Parse();
        }
    }
    VLBI::server->~Server();
    return EXIT_SUCCESS;
}
