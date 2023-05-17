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
#include <thread>

static pthread_mutex_t mutex;
static pthread_mutexattr_t mutexattr;
static bool mutex_initialized = false;
static unsigned long MAX_THREADS = 1;

unsigned long int vlbi_max_threads(unsigned long value)
{
    if(value > 0)
    {
        MAX_THREADS = value;
        dsp_max_threads(value);
    }
    return MAX_THREADS;
}

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

static void lock_mutex()
{
    while(pthread_mutex_trylock(&mutex))
        usleep(100);
}

static void unlock_mutex()
{
    pthread_mutex_unlock(&mutex);
}

static NodeCollection *vlbi_nodes = new NodeCollection();

const char* vlbi_get_version()
{
    return VLBI_VERSION_STRING;
}

static double getDelay(double time, NodeCollection *nodes, VLBIBaseline *b, double Ra, double Dec, double Distance,
                       double wavelength)
{
    double delay = 0.0;
    b->setRa(Ra);
    b->setDec(Dec);
    b->setDistance(Distance);
    b->setRelative(nodes->isRelative());
    b->setWaveLength(wavelength);
    b->setTime(time);
    b->getProjection();
    delay = b->getDelay();
    return delay;
}

double vlbi_get_offset(vlbi_context ctx, double J2000Time, const char* node, double Ra, double Dec, double Distance)
{
    NodeCollection* nodes = (NodeCollection*)ctx;
    char baseline[150];
    BaselineCollection *collection = nodes->getBaselines();
    double offset = 0.0;
    collection->setRa(Ra);
    collection->setDec(Dec);
    collection->setDistance(Distance);
    collection->setFrequency(1);
    VLBIBaseline *bl = nullptr;
    double max_delay = 0;
    int farest = 0, x, y;
    for (x = 0; x < nodes->Count(); x++)
    {
        for (y = x + 1; y < nodes->Count(); y++)
        {
            sprintf(baseline, "%s_%s", nodes->At(x)->getName(), nodes->At(y)->getName());
            if(!collection->Contains(baseline))
                sprintf(baseline, "%s_%s", nodes->At(y)->getName(), nodes->At(x)->getName());
            if(collection->Contains(baseline)) {
                bl = collection->Get(baseline);
                double delay = getDelay(J2000Time, nodes, bl, bl->getRa(), bl->getDec(), bl->getDistance(), bl->getWaveLength());
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
    }
    bl = nullptr;
    sprintf(baseline, "%s_%s", node, nodes->At(farest)->getName());
    if(!collection->Contains(baseline))
        sprintf(baseline, "%s_%s", nodes->At(farest)->getName(), node);
    if(collection->Contains(baseline)) {
        bl = collection->Get(baseline);
        offset = getDelay(J2000Time, nodes, bl, bl->getRa(), bl->getDec(), bl->getDistance(), bl->getWaveLength());
    }
    return offset;
}

static void* fillplane(void *arg)
{
    pfunc;
    struct args
    {
        VLBIBaseline *b;
        NodeCollection *nodes;
        NodeCollection *nodes_2nd;
        BaselineCollection *baselines;
        bool moving_baseline;
        bool nodelay;
        int *stop;
        int *nthreads;
    };
    if(arg == nullptr)return nullptr;
    args *argument = (args*)arg;
    double stack = (*argument->nthreads);
    VLBIBaseline *b = argument->b;
    if(b == nullptr)return nullptr;
    bool moving_baseline = argument->moving_baseline;
    bool nodelay = argument->nodelay;
    NodeCollection *nodes = argument->nodes;
    if(nodes == nullptr)return nullptr;
    NodeCollection *nodes_2nd = argument->nodes_2nd;
    if(nodes_2nd == nullptr)return nullptr;
    BaselineCollection *baselines = argument->baselines;
    if(baselines == nullptr)return nullptr;
    dsp_stream_p parent = baselines->getStream();
    if(parent == nullptr)return nullptr;
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    double st = b->getStartTime();
    double et = b->getEndTime();
    double tau = 1.0 / b->getSampleRate();
    double t;
    int l = 0;
    int e = 0;
    int s = 0;
    int i = 1;
    int *pos = (int*)malloc(sizeof(int)*2);
    double *offsets = (double*)malloc(sizeof(double)*baselines->getCorrelationOrder());
    int idx = 0;
    int oldidx = 0;
    int x;
    double val;
    for(t = st; t < et; t += tau * i, l++)
    {
        if(*argument->stop)
            break;
        for (x = 0; x < nodes->Count(); x++)
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
        for(int y = 0; y < baselines->getCorrelationOrder(); y++) {
            if(nodelay)
            {
                offsets[y] = t;
            }
            else
            {
                offsets[y] = t + vlbi_get_offset((void*)nodes_2nd, t, b->getNode(y)->getName(), b->getRa(), b->getDec(), b->getDistance());
            }
        }
        b->setTime(t);
        b->getProjection();
        int U = (int)b->getU() + u / 2;
        int V = (int)b->getV() + v / 2;
        if(U >= 0 && U < u && V >= 0 && V < v)
        {
            pos[0] = U;
            pos[1] = V;
            idx = dsp_stream_set_position(parent, pos);
            if(idx != oldidx)
            {
                oldidx = idx;
                val = b->Locked() ? b->Correlate(t) : b->Correlate(offsets);
                if(mutex_initialized)
                {
                    lock_mutex();
                    parent->buf[idx] = (parent->buf[idx]+val/stack)/(stack+1);
                    unlock_mutex();
                }
                e = s;
                double k = 100.0 * (t - st) / (et - st);
                pinfo("%.3lf%%\n", k);
            }
        }
        s = l + 1;
        i = s - e;
    }
    free(pos);
    free(offsets);
    (*argument->nthreads)--;
    return nullptr;
}

void* vlbi_init()
{
    init_mutex();
    return new NodeCollection();
}

void vlbi_exit(void* ctx)
{
    NodeCollection *nodes = (NodeCollection*)ctx;
    nodes->~NodeCollection();
    nodes = nullptr;
    destroy_mutex();
}

void vlbi_set_location(void *ctx, double lat, double lon, double el)
{
    pfunc;
    NodeCollection *nodes = (NodeCollection*)ctx;
    nodes->stationLocation()->geographic.lat = lat;
    nodes->stationLocation()->geographic.lon = lon;
    nodes->stationLocation()->geographic.el = el;
    nodes->setRelative(true);
}

void vlbi_add_node(void *ctx, dsp_stream_p stream, const char *name, int geo)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    nodes->Add(new VLBINode(stream, name, nodes->Count(), geo == 1));
}

void vlbi_copy_node(void *ctx, const char *name, const char *node)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(vlbi_has_node(ctx, node)) {
        VLBINode *n = nodes->Get(node);
        nodes->Add(new VLBINode(dsp_stream_copy(n->getStream()), name, nodes->Count(), n->GeographicCoordinates()));
    }
}

dsp_stream_p vlbi_get_node(void *ctx, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    return nodes->Get(name)->getStream();
}

int vlbi_has_node(void *ctx, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    return nodes->Contains(name);
}

int vlbi_get_nodes(void *ctx, vlbi_node** output)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(nodes->Count() > 0)
    {
        vlbi_node* out = (vlbi_node*)malloc(sizeof(vlbi_node) * (size_t)nodes->Count());
        for(int x = 0; x < nodes->Count(); x++)
        {
            out[x].GeographicLocation = nodes->At(x)->getGeographicLocation();
            out[x].Location = nodes->At(x)->getLocation();
            out[x].Geo = nodes->At(x)->GeographicCoordinates();
            out[x].Stream = nodes->At(x)->getStream();
            strcpy(out[x].Name, nodes->At(x)->getName());
            out[x].Index = nodes->At(x)->getIndex();
        }
        *output = out;
        return nodes->Count();
    }
    return 0;
}

void vlbi_del_node(void *ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode* node = nodes->Get(name);
    if(node != nullptr)
    {
        nodes->Remove(name);
        node->~VLBINode();
    }
}

void vlbi_filter_lp_node(void *ctx, const char *name, const char *node, double radians)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode *n = nodes->Get(node);
    dsp_stream_p stream = dsp_stream_copy(n->getStream());
    dsp_fourier_dft(stream, 1);
    dsp_filter_lowpass(stream, radians);
    dsp_stream_free_buffer(stream->magnitude);
    dsp_stream_free_buffer(stream->phase);
    dsp_stream_free(stream->magnitude);
    dsp_stream_free(stream->phase);
    vlbi_add_node(ctx, stream, name, n->GeographicCoordinates());
}

void vlbi_filter_hp_node(void *ctx, const char *name, const char *node, double radians)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode *n = nodes->Get(node);
    dsp_stream_p stream = dsp_stream_copy(n->getStream());
    dsp_fourier_dft(stream, 1);
    dsp_filter_highpass(stream, radians);
    dsp_stream_free_buffer(stream->magnitude);
    dsp_stream_free_buffer(stream->phase);
    dsp_stream_free(stream->magnitude);
    dsp_stream_free(stream->phase);
    vlbi_add_node(ctx, stream, name, n->GeographicCoordinates());
}

void vlbi_filter_bp_node(void *ctx, const char *name, const char *node, double lo_radians, double hi_radians)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode *n = nodes->Get(node);
    dsp_stream_p stream = dsp_stream_copy(n->getStream());
    dsp_fourier_dft(stream, 1);
    dsp_filter_bandpass(stream, lo_radians, hi_radians);
    dsp_stream_free_buffer(stream->magnitude);
    dsp_stream_free_buffer(stream->phase);
    dsp_stream_free(stream->magnitude);
    dsp_stream_free(stream->phase);
    vlbi_add_node(ctx, stream, name, n->GeographicCoordinates());
}

void vlbi_filter_br_node(void *ctx, const char *name, const char *node, double lo_radians, double hi_radians)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode *n = nodes->Get(node);
    dsp_stream_p stream = dsp_stream_copy(n->getStream());
    dsp_fourier_dft(stream, 1);
    dsp_filter_bandreject(stream, lo_radians, hi_radians);
    dsp_stream_free_buffer(stream->magnitude);
    dsp_stream_free_buffer(stream->phase);
    dsp_stream_free(stream->magnitude);
    dsp_stream_free(stream->phase);
    vlbi_add_node(ctx, stream, name, n->GeographicCoordinates());
}

void vlbi_add_model(void *ctx, dsp_stream_p stream, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    nodes->getModels()->Add(stream, name);
}

void vlbi_copy_model(void *ctx, const char *name, const char *model)
{
    pfunc;
    if(vlbi_has_model(ctx, model)) {
        dsp_stream_p stream = vlbi_get_model(ctx, model);
        vlbi_add_model(ctx, dsp_stream_copy(stream), name);
    }
}

int vlbi_get_models(void *ctx, dsp_stream_p** output)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    ModelCollection *models = nodes->getModels();
    if(models->Count() > 0)
    {
        dsp_stream_p* out = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * models->Count());
        for(int x = 0; x < models->Count(); x++)
        {
            out[x] = models->At(x);
        }
        *output = out;
        return models->Count();
    }
    return 0;
}

dsp_stream_p vlbi_get_model(void *ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p model = nodes->getModels()->Get(name);
    if(model != nullptr)
    {
        dsp_buffer_stretch(model->buf, model->len, 0.0, dsp_t_max);
        return model;
    }
    return nullptr;
}

void vlbi_del_model(void *ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(vlbi_has_model(ctx, name))
    {
        dsp_stream_p model = nodes->getModels()->Get(name);
        nodes->getModels()->Remove(name);
        if(model != nullptr) {
            dsp_stream_free_buffer(model);
            dsp_stream_free(model);
        }
    }
}

void vlbi_set_correlation_order(void *ctx, int order)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    nodes->setCorrelationOrder(order);
}

int vlbi_get_baselines(void *ctx, vlbi_baseline** output)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection* baselines = nodes->getBaselines();
    if(baselines->Count() > 0)
    {
        vlbi_baseline* out = (vlbi_baseline*)malloc(sizeof(vlbi_baseline) * baselines->Count());
        for(int x = 0; x < baselines->Count(); x++)
        {
            out[x].nodes_count = baselines->getCorrelationOrder();
            out[x].Nodes = (vlbi_node*)malloc(sizeof(vlbi_node)*out[x].nodes_count);
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
            for(int y = 0; y < out[x].nodes_count; y++) {
                out[x].Nodes[y].GeographicLocation = baselines->At(x)->getNode(y)->getGeographicLocation();
                out[x].Nodes[y].Location = baselines->At(x)->getNode(y)->getLocation();
                out[x].Nodes[y].Geo = baselines->At(x)->getNode(y)->GeographicCoordinates();
                out[x].Nodes[y].Stream = baselines->At(x)->getNode(y)->getStream();
                strcpy(out[x].Nodes[y].Name, baselines->At(x)->getNode(y)->getName());
                out[x].Nodes[y].Index = baselines->At(x)->getNode(y)->getIndex();
            }
            strcpy(out[x].Name, baselines->At(x)->getName());
            out[x].Stream = baselines->At(x)->getStream();
        }
        *output = out;
        return baselines->Count();
    }
    return 0;
}

void vlbi_free_baselines(vlbi_baseline** baselines, int num_baselines)
{
    for(int x = 0; x < num_baselines; x++) {
        free(baselines[x]->Nodes);
    }
    free(baselines);
}

void vlbi_set_baseline_buffer(void *ctx, const char **names, complex_t *buffer, int len)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBIBaseline *b = nullptr;
    BaselineCollection *collection = nodes->getBaselines();
    int x = 0;
    for(x = 0; x < collection->Count(); x++) {
        b = collection->At(x);
        int match = 0;
        for(int y = 0; y < collection->getCorrelationOrder(); y++) {
            for(int z = 0; z < collection->getCorrelationOrder(); z++) {
                if(!strcmp(b->getNode(y)->getName(), names[z])) match ++;
            }
        }
        if(match < collection->getCorrelationOrder()) continue;
        break;
    }
    if(b == nullptr || x == collection->Count()) return;
    dsp_stream_set_dim(b->getStream(), 0, len);
    dsp_stream_alloc_buffer(b->getStream(), len);
    b->getStream()->dft.pairs = buffer;
    b->Lock();
}

void vlbi_set_baseline_stream(void *ctx, const char **names, dsp_stream_p stream)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBIBaseline *b = nullptr;
    BaselineCollection *collection = nodes->getBaselines();
    int x = 0;
    for(x = 0; x < collection->Count(); x++) {
        b = collection->At(x);
        int match = 0;
        for(int y = 0; y < collection->getCorrelationOrder(); y++) {
            for(int z = 0; z < collection->getCorrelationOrder(); z++) {
                if(!strcmp(b->getNode(y)->getName(), names[z])) match ++;
            }
        }
        if(match < collection->getCorrelationOrder()) continue;
        break;
    }
    if(b == nullptr || x == collection->Count()) return;
    b->setStream(stream);
    b->Lock();
}

dsp_stream_p vlbi_get_baseline_stream(void *ctx, const char **names)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBIBaseline *b = nullptr;
    BaselineCollection *collection = nodes->getBaselines();
    int x = 0;
    for(x = 0; x < collection->Count(); x++) {
        b = collection->At(x);
        int match = 0;
        for(int y = 0; y < collection->getCorrelationOrder(); y++) {
            for(int z = 0; z < collection->getCorrelationOrder(); z++) {
                if(!strcmp(b->getNode(y)->getName(), names[z])) match ++;
            }
        }
        if(match < collection->getCorrelationOrder()) continue;
        break;
    }
    if(b == nullptr || x == collection->Count()) return nullptr;
    return b->getStream();
}

void vlbi_unlock_baseline(void *ctx, const char **names)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBIBaseline *b = nullptr;
    BaselineCollection *collection = nodes->getBaselines();
    int x = 0;
    for(x = 0; x < collection->Count(); x++) {
        b = collection->At(x);
        int match = 0;
        for(int y = 0; y < collection->getCorrelationOrder(); y++) {
            for(int z = 0; z < collection->getCorrelationOrder(); z++) {
                if(!strcmp(b->getNode(y)->getName(), names[z])) match ++;
            }
        }
        if(match < collection->getCorrelationOrder()) continue;
        break;
    }
    if(b == nullptr || x == collection->Count()) return;
    b->Unlock();
}

void vlbi_get_uv_plot(vlbi_context ctx, const char *name, int u, int v, double *target, double freq, double sr, int nodelay,
                      int moving_baseline, vlbi_func2_t delegate, int *interrupt)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(nodes == nullptr)return;
    BaselineCollection *baselines = nodes->getBaselines();
    if(baselines == nullptr)return;
    NodeCollection *nodes_2nd = new NodeCollection();
    for(int x = 0; x < nodes->Count(); x++)
        nodes_2nd->Add(nodes->At(x));
    nodes_2nd->setCorrelationOrder(2);
    int stop = 0;
    dsp_stream_p parent = baselines->getStream();
    dsp_buffer_set(parent->buf, parent->len, 0.0);
    baselines->setWidth(u);
    baselines->setHeight(v);
    baselines->setFrequency(freq);
    baselines->setSampleRate(sr);
    baselines->setRa(target[0]);
    baselines->setDec(target[1]);
    baselines->setDistance(target[2]);
    parent->child_count = 0;
    pgarb("%ld nodes, %ld baselines\n", nodes->Count(), baselines->Count());
    baselines->setDelegate(delegate);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * baselines->Count());
    int threads_running = 0;
    int max_threads = (int)vlbi_max_threads(0);
    struct args
    {
        VLBIBaseline *b;
        NodeCollection *nodes;
        NodeCollection *nodes_2nd;
        BaselineCollection *baselines;
        bool moving_baseline;
        bool nodelay;
        int *stop;
        int *nthreads;
    };
    args *argument = (args*)malloc(sizeof(args) * (size_t)baselines->Count());
    for(int i = 0; i < baselines->Count(); i++)
    {
        VLBIBaseline *b = baselines->At(i);
        if(b == nullptr)continue;
        argument[i].b = b;
        argument[i].nodes = nodes;
        argument[i].nodes_2nd = nodes_2nd;
        argument[i].baselines = baselines;
        argument[i].moving_baseline = moving_baseline;
        argument[i].nodelay = nodelay;
        argument[i].nthreads = &threads_running;
        if(interrupt != nullptr)
            argument[i].stop = interrupt;
        else
            argument[i].stop = &stop;
        while(threads_running > max_threads - 1)
            usleep(100000);
        threads_running++;
        pthread_create(&threads[i], &attr, fillplane, &argument[i]);
    }
    for(int i = 0; i < baselines->Count(); i++)
        pthread_join(threads[i], nullptr);
    free(argument);
    free(threads);
    pthread_attr_destroy(&attr);
    if(vlbi_has_model(ctx, name)) {
        dsp_stream_p model = vlbi_get_model(ctx, name);
        dsp_stream_set_dim(model, 0, u);
        dsp_stream_set_dim(model, 1, v);
        dsp_stream_alloc_buffer(model, model->len);
        dsp_buffer_copy(parent->buf, model->buf, model->len);
    } else
        vlbi_add_model(ctx, dsp_stream_copy(parent), name);
    pgarb("aperture synthesis plotting completed\n");
}

void vlbi_get_ifft(vlbi_context ctx, const char *name, const char *magnitude, const char *phase)
{
    pfunc;
    if(!vlbi_has_model(ctx, magnitude))
        return;
    if(!vlbi_has_model(ctx, phase))
        return;
    dsp_stream_p mag = vlbi_get_model(ctx, magnitude);
    dsp_stream_p phi = vlbi_get_model(ctx, phase);
    int d = 0;
    if(mag->dims == phi->dims)
        for (d = 0; d < mag->dims && mag->sizes[d] == phi->sizes[d]; )
            d++;
    if(mag->dims == d)
    {
        dsp_stream_p ifft = nullptr;
        if(vlbi_has_model(ctx, name))
            ifft = vlbi_get_model(ctx, name);
        else
            ifft = dsp_stream_copy(mag);
        ifft->phase = dsp_stream_copy(phi);
        ifft->magnitude = dsp_stream_copy(mag);
        dsp_buffer_stretch(ifft->phase->buf, ifft->phase->len, 0, PI * 2.0);
        dsp_buffer_stretch(ifft->magnitude->buf, ifft->magnitude->len, 0, dsp_t_max);
        dsp_buffer_set(ifft->buf, ifft->len, 0.0);
        ifft->buf[0] = dsp_t_max;
        dsp_fourier_idft(ifft);
        dsp_stream_free_buffer(ifft->phase);
        dsp_stream_free(ifft->phase);
        dsp_stream_free_buffer(ifft->magnitude);
        dsp_stream_free(ifft->magnitude);
        if(!vlbi_has_model(ctx, name))
            vlbi_add_model(ctx, ifft, name);
    }
}

void vlbi_get_fft(vlbi_context ctx, const char *name, const char *magnitude, const char *phase)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, name))
        return;
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
    if(!vlbi_has_model(ctx, stream))
        return;
    if(!vlbi_has_model(ctx, mask))
        return;
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
}

void vlbi_apply_convolution_matrix(vlbi_context ctx, const char *name, const char *model, const char *matrix)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, model))
        return;
    if(!vlbi_has_model(ctx, matrix))
        return;
    dsp_stream_p convoluted = dsp_stream_copy(nodes->getModels()->Get(model));
    dsp_stream_p convolution = nodes->getModels()->Get(matrix);
    if(convoluted->dims == convolution->dims)
    {
        dsp_fourier_dft(convoluted, 1);
        dsp_fourier_dft(convolution, 1);
        dsp_convolution_convolution(convoluted, convolution);
        vlbi_add_model(ctx, convoluted, name);
        dsp_stream_free_buffer(convoluted->magnitude);
        dsp_stream_free(convoluted->magnitude);
        dsp_stream_free_buffer(convoluted->phase);
        dsp_stream_free(convoluted->phase);
        dsp_stream_free_buffer(convolution->magnitude);
        dsp_stream_free(convolution->magnitude);
        dsp_stream_free_buffer(convolution->phase);
        dsp_stream_free(convolution->phase);
    }
}

void vlbi_stack_models(vlbi_context ctx, const char *name, const char *model1, const char *model2)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, model1))
        return;
    if(!vlbi_has_model(ctx, model2))
        return;
    dsp_stream_p stacked = dsp_stream_copy(nodes->getModels()->Get(model1));
    dsp_stream_p model = nodes->getModels()->Get(model2);
    if(stacked->dims == model->dims)
    {
        dsp_buffer_div1(stacked, 2);
        dsp_buffer_div1(model, 2);
        dsp_buffer_sum(stacked, model->buf, fmin(stacked->len, model->len));
        vlbi_add_model(ctx, stacked, name);
    }
}

void vlbi_diff_models(vlbi_context ctx, const char *name, const char *model1, const char *model2)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, model1))
        return;
    if(!vlbi_has_model(ctx, model2))
        return;
    dsp_stream_p diff = dsp_stream_copy(nodes->getModels()->Get(model1));
    dsp_stream_p model = nodes->getModels()->Get(model2);
    dsp_t mn = dsp_stats_min(model->buf, model->len);
    dsp_t mx = dsp_stats_max(model->buf, model->len);
    if(diff->dims == model->dims)
    {
        dsp_buffer_sub(diff, model->buf, fmin(diff->len, model->len));
        dsp_buffer_stretch(diff->buf, diff->len, mn, mx);
        vlbi_add_model(ctx, diff, name);
    }
}

void vlbi_shift(vlbi_context ctx, const char *name)
{
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    if(!vlbi_has_model(ctx, name))
        return;
    dsp_stream_p shifted = nodes->getModels()->Get(name);
    dsp_buffer_shift(shifted);
}

int vlbi_has_model(void *ctx, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    return nodes->getModels()->Contains(name);
}

void vlbi_add_model_from_png(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int channels;
    dsp_stream_p* file = nullptr;
    file = dsp_file_read_png(filename, &channels, 0);
    char *model = (char*)malloc(strlen(name)+5);
    if(file != nullptr)
    {
        if(channels > 1) {
            for(int c = 0; c < channels; c++)
            {
                sprintf(model, "%s_%d", name, c);
                vlbi_add_model(nodes, file[c], model);
            }
        }
        vlbi_add_model(nodes, dsp_stream_copy(file[channels]), name);
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
    char *model = (char*)malloc(strlen(name)+5);
    if(file != nullptr)
    {
        if(channels > 1) {
            for(int c = 0; c < channels; c++)
            {
                sprintf(model, "%s_%d", name, c);
                vlbi_add_model(nodes, file[c], model);
            }
        }
        vlbi_add_model(nodes, file[channels], name);
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
    char *model = (char*)malloc(strlen(name)+5);
    if(file != nullptr)
    {
        if(channels > 1) {
            for(int c = 0; c < channels; c++)
            {
                sprintf(model, "%s_%d", name, c);
                vlbi_add_model(nodes, file[c], model);
            }
        }
        vlbi_add_model(nodes, dsp_stream_copy(file[channels]), name);
        free(file);
    }
}

void vlbi_get_model_to_png(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int components = 0;
    dsp_stream_p* file = (dsp_stream_p*)malloc(sizeof(dsp_stream_p));
    char *modelname = (char*)malloc(strlen(name)+5);
    int c = 0;
    sprintf(modelname, "%s_%d", name, c);
    dsp_stream_p model = nodes->getModels()->Get(modelname);
    if(model == nullptr) {
        file = (dsp_stream_p*)realloc(file, sizeof(dsp_stream_p)* (size_t)(components+1));
        model = nodes->getModels()->Get(name);
        if(model != nullptr)
            file[components++] = model;
    }
    while(model != nullptr) {
        file = (dsp_stream_p*)realloc(file, sizeof(dsp_stream_p)* (size_t)(components+1));
        sprintf(modelname, "%s_%d", name, components);
        file[components++] = model;
        model = nodes->getModels()->Get(modelname);
    }
    if(components > 0)
        dsp_file_write_png_composite(filename, components, 9, file);
    free(file);
}

void vlbi_get_model_to_jpeg(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int components = 0;
    dsp_stream_p* file = (dsp_stream_p*)malloc(sizeof(dsp_stream_p));
    char *modelname = (char*)malloc(strlen(name)+5);
    int c = 0;
    sprintf(modelname, "%s_%d", name, c);
    dsp_stream_p model = nodes->getModels()->Get(modelname);
    if(model == nullptr) {
        file = (dsp_stream_p*)realloc(file, sizeof(dsp_stream_p)* (size_t)(components+1));
        model = nodes->getModels()->Get(name);
        if(model != nullptr)
            file[components++] = model;
    }
    while(model != nullptr) {
        file = (dsp_stream_p*)realloc(file, sizeof(dsp_stream_p)* (size_t)(components+1));
        sprintf(modelname, "%s_%d", name, components);
        file[components++] = model;
        model = nodes->getModels()->Get(modelname);
    }
    if(components > 0)
        dsp_file_write_jpeg_composite(filename, components, 90, file);
    free(file);
}

void vlbi_get_model_to_fits(void *ctx, char *filename, const char *name)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    int components = 0;
    dsp_stream_p* file = (dsp_stream_p*)malloc(sizeof(dsp_stream_p));
    char *modelname = (char*)malloc(strlen(name)+5);
    int c = 0;
    sprintf(modelname, "%s_%d", name, c);
    dsp_stream_p model = nodes->getModels()->Get(modelname);
    if(model == nullptr) {
        file = (dsp_stream_p*)realloc(file, sizeof(dsp_stream_p)* (size_t)(components+1));
        model = nodes->getModels()->Get(name);
        if(model != nullptr)
            file[components++] = model;
    }
    while(model != nullptr) {
        file = (dsp_stream_p*)realloc(file, sizeof(dsp_stream_p)* (size_t)(components+1));
        sprintf(modelname, "%s_%d", name, components);
        file[components++] = model;
        model = nodes->getModels()->Get(modelname);
    }
    if(components > 0)
        dsp_file_write_fits_composite(filename, components, 16, file);
    free(file);
}

void vlbi_add_node_from_fits(void *ctx, char *filename, const char *name, int geo)
{
    pfunc;
    NodeCollection *nodes = (ctx != nullptr) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p stream = vlbi_file_read_fits(filename);
    if(stream != nullptr)
        nodes->Add(new VLBINode(stream, name, nodes->Count(), geo == 1));
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
            nodes->Add(new VLBINode(stream[i], name, nodes->Count(), geo == 1));
    }
}
