/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2022  Ilia Platone
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nodecollection.h>
#include <baselinecollection.h>
#include <modelcollection.h>
#include <base64.h>

static pthread_mutex_t mutex;
static pthread_mutexattr_t mutexattr;
static bool mutex_initialized = false;

static void init_mutex()
{
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &mutexattr);
    mutex_initialized = true;
}

static void destroy_mutex()
{
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&mutexattr);
    mutex_initialized = false;
}

static NodeCollection *vlbi_nodes = new NodeCollection();

typedef struct _vlbi_thread_t
{
    void *(*__start_routine) (void *);
    void* arg;
    int* thread_cnt;
    pthread_t th;
} vlbi_thread_t;

const char* vlbi_get_version()
{
    return VLBI_VERSION_STRING;
}

static void vlbi_wait_threads(int *thread_cnt)
{
    while(((int)*thread_cnt) > 0)
    {
        usleep(100000);
    }
}

static void* vlbi_thread_func(void* arg)
{
    vlbi_thread_t *t = ((vlbi_thread_t*)arg);
    *t->thread_cnt = *t->thread_cnt + 1;
    t->__start_routine(t->arg);
    *t->thread_cnt = *t->thread_cnt - 1;
    free(t);
    return nullptr;
}

static void vlbi_start_thread(void *(*__start_routine) (void *), void *arg, int *thread_cnt)
{
    vlbi_thread_t *t = (vlbi_thread_t*)malloc(sizeof(vlbi_thread_t));
    memset(t, 0, sizeof(vlbi_thread_t));
    t->__start_routine = __start_routine;
    t->arg = arg;
    t->thread_cnt = thread_cnt;

    usleep(100000);
    pthread_create(&t->th, nullptr, &vlbi_thread_func, t);
}

static double getDelay(double time, NodeCollection *nodes, VLBINode *n1, VLBINode *n2, double Ra, double Dec,
                       double wavelength)
{
    VLBIBaseline *b = new VLBIBaseline(n1, n2);
    b->setRa(Ra);
    b->setDec(Dec);
    b->setRelative(nodes->isRelative());
    b->setWaveLength(wavelength);
    b->setTime(time);
    b->getProjection();
    double delay = b->getDelay();
    b->~VLBIBaseline();
    return delay;
}

void vlbi_get_offsets(vlbi_context ctx, double J200Time, const char* node1, const char* node2, double Ra, double Dec,
                      double *offset1,
                      double *offset2)
{
    NodeCollection* nodes = (NodeCollection*)ctx;
    char baseline[150];
    sprintf(baseline, "%s_%s", node1, node2);
    VLBIBaseline *b = nodes->getBaselines()->Get(baseline);
    b->setRa(Ra);
    b->setDec(Dec);
    double max_delay = 0;
    int farest = 0, x, y;
    for (x = 0; x < nodes->Count; x++)
    {
        for (y = x + 1; y < nodes->Count; y++)
        {
            double delay = getDelay(J200Time, nodes, nodes->At(x), nodes->At(y), b->getRa(), b->getDec(), b->getWaveLength());
            if(fabs(delay) > max_delay)
            {
                max_delay = fabs(delay);
                if(delay < 0)
                    farest = x;
                else
                    farest = y;
            }
        }
    }
    *offset1 = getDelay(J200Time, nodes, b->getNode1(), nodes->At(farest), b->getRa(), b->getDec(), b->getWaveLength());
    *offset2 = getDelay(J200Time, nodes, b->getNode2(), nodes->At(farest), b->getRa(), b->getDec(), b->getWaveLength());
}

static void* fillplane(void *arg)
{
    pfunc;
    struct args
    {
        VLBIBaseline *b;
        NodeCollection *nodes;
        bool moving_baseline;
        bool nodelay;
    };
    args *argument = (args*)arg;
    VLBIBaseline *b = argument->b;
    bool moving_baseline = argument->moving_baseline;
    bool nodelay = argument->nodelay;
    NodeCollection *nodes = argument->nodes;
    dsp_stream_p parent = nodes->getBaselines()->getStream();
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    double st = b->getStartTime();
    double et = b->getEndTime();
    double tau = 1.0 / b->getSampleRate();
    double time;
    int l = 0;
    int e = 0;
    int s = 0;
    int i = 1;
    double offset1;
    double offset2;
    int idx = 0;
    int oldidx = 0;
    int x;
    double val;
    for(time = st; time < et; time += tau * i, l++)
    {
        for (x = 0; x < nodes->Count; x++)
        {
            if(moving_baseline)
            {
                nodes->At(x)->setLocation(l);
            }
            else
            {
                nodes->At(x)->setLocation(0);
            }
        }
        if(nodelay)
        {
            offset1 = offset2 = 0.0;
        }
        else
        {
            vlbi_get_offsets((void*)nodes, time, b->getNode1()->getName(), b->getNode2()->getName(), b->getRa(), b->getDec(), &offset1,
                             &offset2);
        }
        b->setTime(time);
        b->getProjection();
        int U = (int)b->getU() + u / 2;
        int V = (int)b->getV() + v / 2;
        if(U >= 0 && U < u && V >= 0 && V < v)
        {
            idx = (int)(U + V * u);
            if(idx != oldidx)
            {
                oldidx = idx;
                val = b->Locked() ? b->Correlate(time) : b->Correlate(time + offset1, time + offset2);
                if(mutex_initialized)
                {
                    while(pthread_mutex_trylock(&mutex))
                        usleep(10);
                    parent->buf[idx] = val;
                    pthread_mutex_unlock(&mutex);
                }
                e = s;
                fprintf(stderr, "\r%.3lfs %.3lf %d  ", (time - st), (time - st) * 100.0 / (et - st - tau), i);
            }
        }
        s = l + 1;
        i = s - e;
    }
    return nullptr;
}

void* vlbi_init()
{
    return new NodeCollection();
}

void vlbi_exit(void* ctx)
{
    NodeCollection *nodes = (NodeCollection*)ctx;
    nodes->~NodeCollection();
    nodes = nullptr;
}

void vlbi_set_location(void *ctx, double lat, double lon, double el)
{
    pfunc;
    NodeCollection *nodes = (NodeCollection*)ctx;
    nodes->stationLocation()->geographic.lat = lat;
    nodes->stationLocation()->geographic.lon = lon;
    nodes->stationLocation()->geographic.el = el;
    nodes->setRelative(true);
    nodes->getBaselines()->setRelative(true);
}

void vlbi_add_node(void *ctx, dsp_stream_p stream, const char *name, int geo)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    nodes->Add(new VLBINode(stream, name, nodes->Count, geo == 1));
}

int vlbi_get_nodes(void *ctx, vlbi_node** output)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(nodes->Count > 0)
    {
        vlbi_node* out = (vlbi_node*)malloc(sizeof(vlbi_node) * (size_t)nodes->Count);
        for(int x = 0; x < nodes->Count; x++)
        {
            out[x].GeographicLocation = nodes->At(x)->getGeographicLocation();
            out[x].Location = nodes->At(x)->getLocation();
            out[x].Geo = nodes->At(x)->GeographicCoordinates();
            out[x].Stream = nodes->At(x)->getStream();
            strcpy(out[x].Name, nodes->At(x)->getName());
            out[x].Index = nodes->At(x)->getIndex();
        }
        *output = out;
        return nodes->Count;
    }
    return 0;
}

void vlbi_del_node(void *ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode* node = nodes->Get(name);
    nodes->Remove(node);
    node->~VLBINode();
}

void vlbi_add_model(void *ctx, dsp_stream_p stream, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    nodes->getModels()->Add(stream, name);
}

int vlbi_get_models(void *ctx, dsp_stream_p** output)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    ModelCollection *models = nodes->getModels();
    if(models->Count > 0)
    {
        dsp_stream_p* out = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * models->Count);
        for(int x = 0; x < models->Count; x++)
        {
            out[x] = models->At(x);
        }
        *output = out;
        return models->Count;
    }
    return 0;
}

dsp_stream_p vlbi_get_model(void *ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p model = nodes->getModels()->Get(name);
    dsp_buffer_stretch(model->buf, model->len, 0.0, dsp_t_max);
    return model;
}

void vlbi_del_model(void *ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p model = nodes->getModels()->Get(name);
    nodes->getModels()->Remove(model);
    dsp_stream_free_buffer(model);
    dsp_stream_free(model);
}

int vlbi_get_baselines(void *ctx, vlbi_baseline** output)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection* baselines = nodes->getBaselines();
    if(baselines->Count > 0)
    {
        vlbi_baseline* out = (vlbi_baseline*)malloc(sizeof(vlbi_baseline) * baselines->Count);
        for(int x = 0; x < baselines->Count; x++)
        {
            out[x].relative = baselines->At(x)->isRelative();
            out[x].locked = baselines->At(x)->Locked();
            out[x].Target = baselines->At(x)->getTarget();
            out[x].Ra = baselines->At(x)->getRa();
            out[x].Dec = baselines->At(x)->getDec();
            out[x].baseline = baselines->At(x)->getBaseline();
            out[x].u = baselines->At(x)->getU();
            out[x].v = baselines->At(x)->getV();
            out[x].delay = baselines->At(x)->getDelay();
            out[x].WaveLength = baselines->At(x)->getWaveLength();
            out[x].SampleRate = baselines->At(x)->getSampleRate();
            out[x].Node1.GeographicLocation = baselines->At(x)->getNode1()->getGeographicLocation();
            out[x].Node1.Location = baselines->At(x)->getNode1()->getLocation();
            out[x].Node1.Geo = baselines->At(x)->getNode1()->GeographicCoordinates();
            out[x].Node1.Stream = baselines->At(x)->getNode1()->getStream();
            strcpy(out[x].Node1.Name, baselines->At(x)->getNode1()->getName());
            out[x].Node1.Index = baselines->At(x)->getNode1()->getIndex();
            out[x].Node2.GeographicLocation = baselines->At(x)->getNode2()->getGeographicLocation();
            out[x].Node2.Location = baselines->At(x)->getNode2()->getLocation();
            out[x].Node2.Geo = baselines->At(x)->getNode2()->GeographicCoordinates();
            out[x].Node2.Stream = baselines->At(x)->getNode2()->getStream();
            strcpy(out[x].Node2.Name, baselines->At(x)->getNode2()->getName());
            out[x].Node2.Index = baselines->At(x)->getNode2()->getIndex();
            strcpy(out[x].Name, baselines->At(x)->getName());
            out[x].Stream = baselines->At(x)->getStream();
        }
        *output = out;
        return baselines->Count;
    }
    return 0;
}

void vlbi_set_baseline_buffer(void *ctx, const char *node1, const char *node2, fftw_complex *buffer, int len)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    char name[150];
    sprintf(name, "%s_%s", node1, node2);
    VLBIBaseline *b = nodes->getBaselines()->Get(name);
    b->getStream()->sizes[0] = len;
    b->getStream()->len = len;
    dsp_stream_alloc_buffer(b->getStream(), b->getStream()->len);
    dsp_buffer_copy(((double*)buffer), b->getStream()->dft.buf, len);
    dsp_fourier_2dsp(b->getStream());
    b->Lock();
}

void vlbi_get_uv_plot(vlbi_context ctx, const char *name, int u, int v, double *target, double freq, double sr, int nodelay,
                      int moving_baseline, vlbi_func2_t delegate)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    pgarb("%d nodes, %d baselines\n", nodes->Count, nodes->Count * (nodes->Count - 1) / 2);
    BaselineCollection *baselines = nodes->getBaselines();
    baselines->setWidth(u);
    baselines->setHeight(v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->setRa(target[0]);
    baselines->setDec(target[1]);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    pgarb("%d nodes\n%d baselines\n", nodes->Count, baselines->Count);
    baselines->SetDelegate(delegate);
    init_mutex();
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        struct args
        {
            VLBIBaseline *b;
            NodeCollection *nodes;
            bool moving_baseline;
            bool nodelay;
        };
        args argument;
        argument.b = b;
        argument.nodes = nodes;
        argument.moving_baseline = moving_baseline;
        argument.nodelay = nodelay;
        vlbi_start_thread(fillplane, &argument, &parent->child_count);
    }
    vlbi_wait_threads(&parent->child_count);
    destroy_mutex();
    pgarb("aperture synthesis plotting completed\n");
    vlbi_add_model(ctx, parent, name);
}

void vlbi_get_ifft(vlbi_context ctx, const char *name, const char *magnitude, const char *phase)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p mag = nodes->getModels()->Get(magnitude);
    dsp_stream_p phi = nodes->getModels()->Get(phase);
    int d = 0;
    if(mag->dims == phi->dims)
        for (d = 0; d < mag->dims && mag->sizes[d] == phi->sizes[d]; )
            d++;
    if(mag->dims == d)
    {
        dsp_buffer_stretch(phi->buf, phi->len, 0, PI * 2.0);
        dsp_buffer_stretch(mag->buf, mag->len, 0, dsp_t_max);
        dsp_stream_p ifft = dsp_stream_copy(mag);
        ifft->phase = phi;
        ifft->magnitude = mag;
        dsp_fourier_idft(ifft);
        vlbi_add_model(ctx, ifft, name);
    }
}

void vlbi_get_fft(vlbi_context ctx, const char *name, const char *magnitude, const char *phase)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p fft = nodes->getModels()->Get(name);
    fft->phase = nullptr;
    fft->magnitude = nullptr;
    dsp_fourier_dft(fft, 1);
    vlbi_add_model(ctx, fft->phase, phase);
    vlbi_add_model(ctx, fft->magnitude, magnitude);
}

void vlbi_apply_mask(vlbi_context ctx, const char *name, const char *stream, const char *mask)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p masked = dsp_stream_copy(nodes->getModels()->Get(stream));
    dsp_stream_p model = nodes->getModels()->Get(mask);
    int d = 0;
    if(masked->dims == model->dims)
    {
        for (d = 0; d < masked->dims && masked->sizes[d] == model->sizes[d]; )
            d++;
        if(masked->dims == d)
        {
            dsp_buffer_stretch(model->buf, model->len, 0.0, 1.0);
            dsp_buffer_mul(masked, model->buf, model->len);
            vlbi_add_model(ctx, masked, name);
            return;
        }
    }
    dsp_stream_free_buffer(masked);
    dsp_stream_free(masked);
}

void vlbi_shift(vlbi_context ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p shifted = nodes->getModels()->Get(name);
    dsp_buffer_shift(shifted);
}

void vlbi_add_model_from_png(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int channels;
    dsp_stream_p* file = nullptr;
    file = dsp_file_read_png(filename, &channels, 0);
    if(file != nullptr)
    {
        nodes->getModels()->Add(dsp_stream_copy(file[channels]), name);
        for(int c = 0; c < channels; c++)
        {
            dsp_stream_free_buffer(file[c]);
            dsp_stream_free(file[c]);
        }
        free(file);
    }
}

void vlbi_add_model_from_jpeg(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int channels;
    dsp_stream_p* file = nullptr;
    file = dsp_file_read_jpeg(filename, &channels, 0);
    if(file != nullptr)
    {
        nodes->getModels()->Add(dsp_stream_copy(file[channels]), name);
        for(int c = 0; c < channels; c++)
        {
            dsp_stream_free_buffer(file[c]);
            dsp_stream_free(file[c]);
        }
        free(file);
    }
}

void vlbi_add_model_from_fits(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int channels;
    dsp_stream_p* file = nullptr;
    file = dsp_file_read_fits(filename, &channels, 0);
    if(file != nullptr)
    {
        nodes->getModels()->Add(dsp_stream_copy(file[channels]), name);
        for(int c = 0; c < channels; c++)
        {
            dsp_stream_free_buffer(file[c]);
            dsp_stream_free(file[c]);
        }
        free(file);
    }
}

void vlbi_get_model_to_png(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int components = 1;
    dsp_stream_p* file = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * (size_t)components);
    dsp_stream_p model = nodes->getModels()->Get(name);
    for(int c = 0; c < components; c++)
        file[c] = model;
    dsp_file_write_png_composite(filename, components, 9, file);
    free(file);
}

void vlbi_get_model_to_jpeg(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int components = 1;
    dsp_stream_p* file = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * (size_t)components);
    dsp_stream_p model = nodes->getModels()->Get(name);
    for(int c = 0; c < components; c++)
        file[c] = model;
    dsp_file_write_jpeg_composite(filename, components, 90, file);
    free(file);
}

void vlbi_get_model_to_fits(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int components = 1;
    dsp_stream_p* file = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * (size_t)components);
    dsp_stream_p model = nodes->getModels()->Get(name);
    for(int c = 0; c < components; c++)
        file[c] = model;
    dsp_file_write_fits_composite(filename, components, 16, file);
    free(file);
}

void vlbi_add_node_from_fits(void *ctx, char *filename, const char *name, int geo)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p stream = vlbi_file_read_fits(filename);
    if(stream != nullptr)
        nodes->Add(new VLBINode(stream, name, nodes->Count, geo == 1));
}

void vlbi_add_nodes_from_sdfits(void *ctx, char *filename, const char *name, int geo)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    long n = 0;
    dsp_stream_p *stream = vlbi_file_read_sdfits(filename, &n);
    if(stream != nullptr)
    {
        for(int i = 0; i < n; i++)
            nodes->Add(new VLBINode(stream[i], name, nodes->Count, geo == 1));
    }
}
