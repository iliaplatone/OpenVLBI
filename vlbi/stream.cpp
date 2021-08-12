/*  OpenVLBI - Open Source Very Long Baseline Interferometry
    Copyright © 2017-2019  Ilia Platone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <linux/fcntl.h>
#include <string.h>
#include <pthread.h>
#include <nodecollection.h>
#include <baselinecollection.h>
#include <base64.h>

#define THREADS_MASK ((1<<vlbi_max_threads(0))-1)
unsigned long int MAX_THREADS = 48;
int DSP_MAX_THREADS = MAX_THREADS;

static NodeCollection *vlbi_nodes = new NodeCollection();

typedef struct _vlbi_thread_t {
    void *(*__start_routine) (void *);
    void* arg;
    unsigned long m;
    int* thread_cnt;
    pthread_t th;
}vlbi_thread_t;

char* vlbi_get_version() { return (char*)VLBI_VERSION_STRING; }

static void vlbi_wait_threads(int *thread_cnt)
{
    while(((int)*thread_cnt) > 0) {
        usleep(100000);
    }
}

static void* vlbi_thread_func(void* arg)
{
    vlbi_thread_t *t = (vlbi_thread_t*)arg;
    t->__start_routine(t->arg);
    *t->thread_cnt = (((int)(*t->thread_cnt))&t->m);
    free(t);
    return NULL;
}

static void vlbi_start_thread(void *(*__start_routine) (void *), void *arg, int *thread_cnt, int n)
{
    unsigned long nt = (1<<(n%vlbi_max_threads(0)));
    vlbi_thread_t *t = (vlbi_thread_t *)malloc(sizeof(vlbi_thread_t));
    t->__start_routine = __start_routine;
    t->arg = arg;
    t->m = (nt^THREADS_MASK);
    t->thread_cnt = thread_cnt;

    usleep(100000);
    if(((int)(*t->thread_cnt))==THREADS_MASK)
        vlbi_wait_threads(t->thread_cnt);
    *t->thread_cnt = (((int)(*t->thread_cnt))|nt);
    pthread_create(&t->th, NULL, &vlbi_thread_func, t);
}

static double getDelay(double time, NodeCollection *nodes, VLBINode *n1, VLBINode *n2, double Ra, double Dec, double wavelength)
{
    double Alt, Az;
    double center[2];
    VLBIBaseline *b = new VLBIBaseline(n1 ,n2);
    b->setRelative(nodes->isRelative());
    b->setWaveLength(wavelength);
    if(!nodes->isRelative()) {
        center[0] = b->getNode2()->getGeographicLocation()[0]+b->getNode1()->getGeographicLocation()[0]/2;
        center[1] = b->getNode2()->getGeographicLocation()[1]+b->getNode1()->getGeographicLocation()[1]/2;
        while(center[0] > 90.0)
            center[0] -= 180.0;
        while(center[0] < -90.0)
            center[0] += 180.0;
        while(center[1] > 180.0)
            center[1] -= 360.0;
        while(center[1] < -180.0)
            center[1] += 360.0;
        vlbi_astro_alt_az_from_ra_dec(vlbi_time_J2000time_to_lst(time, b->getNode2()->getGeographicLocation()[1]), Ra, Dec, center[0], center[1], &Alt, &Az);
    } else {
        vlbi_astro_alt_az_from_ra_dec(vlbi_time_J2000time_to_lst(time, nodes->getLocation()->geographic.lon), Ra, Dec, nodes->getLocation()->geographic.lat, nodes->getLocation()->geographic.lon, &Alt, &Az);
    }
    b->setTarget(Az, Alt);
    b->getProjection();
    double delay = b->getDelay();
    b->~VLBIBaseline();
    return delay;
}

void vlbi_get_offsets(vlbi_context ctx, double J200Time, char* node1, char* node2, double Ra, double Dec, double *offset1, double *offset2)
{
    NodeCollection* nodes = (NodeCollection*)ctx;
    char baseline[150];
    sprintf(baseline, "%s_%s", node1, node2);
    VLBIBaseline *b = nodes->getBaselines()->Get(baseline);
    b->setRa(Ra);
    b->setDec(Dec);
    double max_delay = 0, Alt, Az;
    double center[3];
    int farest = 0, x, y;
    for (x = 0; x < nodes->Count; x++) {
        for (y = x+1; y < nodes->Count; y++) {
            double delay = getDelay(J200Time, nodes, nodes->At(x), nodes->At(y), b->getRa(), b->getDec(), b->getWaveLength());
            if(fabs(delay)>max_delay) {
                max_delay = fabs(delay);
                if(delay < 0)
                    farest = x;
                else
                    farest = y;
            }
        }
    }
    if(!nodes->isRelative()) {
        center[0] = b->getNode2()->getGeographicLocation()[0]+b->getNode1()->getGeographicLocation()[0]/2;
        center[1] = b->getNode2()->getGeographicLocation()[1]+b->getNode1()->getGeographicLocation()[1]/2;
        while(center[0] > 90.0)
            center[0] -= 180.0;
        while(center[0] < -90.0)
            center[0] += 180.0;
        while(center[1] > 180.0)
            center[1] -= 360.0;
        while(center[1] < -180.0)
            center[1] += 360.0;
        vlbi_astro_alt_az_from_ra_dec(vlbi_time_J2000time_to_lst(J200Time, b->getNode2()->getGeographicLocation()[1]), b->getRa(), b->getDec(), center[0], center[1], &Alt, &Az);
    } else {
        vlbi_astro_alt_az_from_ra_dec(vlbi_time_J2000time_to_lst(J200Time, nodes->getLocation()->geographic.lon), b->getRa(), b->getDec(), nodes->getLocation()->geographic.lat, nodes->getLocation()->geographic.lon, &Alt, &Az);
    }
    b->setTarget(Az, Alt);
    *offset1 = getDelay(J200Time, nodes, b->getNode1(), nodes->At(farest), b->getRa(), b->getDec(), b->getWaveLength());
    *offset2 = getDelay(J200Time, nodes, b->getNode2(), nodes->At(farest), b->getRa(), b->getDec(), b->getWaveLength());
}

static void* fillplane(void *arg)
{
    struct args { VLBIBaseline *b; NodeCollection *nodes; bool moving_baseline; bool nodelay; };
    args *argument = (args*)arg;
    VLBIBaseline *b = argument->b;
    bool moving_baseline = argument->moving_baseline;
    bool nodelay = argument->nodelay;
    NodeCollection *nodes = argument->nodes;
    dsp_stream_p s = b->getStream();
    dsp_stream_p parent = (dsp_stream_p)nodes->getBaselines()->getStream();
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    double st = b->getStartTime();
    double et = b->getEndTime();
    double tau = 1.0/b->getSampleRate();
    double time;
    int l = 0;
    double max_delay = 0;
    int farest = 0;
    double center[3];
    double Alt, Az;
    double offset1;
    double offset2;
    int U;
    int V;
    int idx;
    int x, y;
    double val;
    for(time = st; time < et; time += tau, l++) {
        for (x = 0; x < nodes->Count; x++){
            if(moving_baseline) {
                nodes->At(x)->setLocation(l);
            } else {
                nodes->At(x)->setLocation(0);
            }
        }
        vlbi_get_offsets((void*)nodes, time, b->getNode1()->getName(), b->getNode2()->getName(), b->getRa(), b->getDec(), &offset1, &offset2);
        if(nodelay) {
            offset1 = offset2 = 0.0;
        }
        b->getProjection();
        int U = b->getU() + u / 2;
        int V = b->getV() + v / 2;
        if(U >= 0 && U < u && V >= 0 && V < v) {
            idx = (int)(U+V*u);
            val = b->Locked() ? b->Correlate(time) : b->Correlate(time+offset1, time+offset2);
            parent->buf[idx] = val;
            parent->buf[parent->len-idx] = val;
        }
        fprintf(stderr, "\r%.3fs %.3f%%   ", (time-st), (time-st)*100.0/(et-st-tau));
    }
    return NULL;
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
    NodeCollection *nodes = (NodeCollection*)ctx;
    nodes->getLocation()->geographic.lat = lat;
    nodes->getLocation()->geographic.lon = lon;
    nodes->getLocation()->geographic.el = el;
    nodes->setRelative(true);
    nodes->getBaselines()->setRelative(true);
}

void vlbi_add_stream(void *ctx, dsp_stream_p Stream, char* name, int geo) {
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p stream = dsp_stream_copy(Stream);
    nodes->Add(new VLBINode(stream, name, nodes->Count, geo == 1));
}

void vlbi_del_stream(void *ctx, char* name) {
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    VLBINode* node = nodes->Get(name);
    nodes->Remove(node);
    node->~VLBINode();
}

void vlbi_set_baseline_buffer(void *ctx, char* node1, char* node2, dsp_t *buffer, int len) {
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    int idx = (nodes->Get(node1)->getIndex()*2-1)*nodes->Get(node1)->getIndex()/2+nodes->Get(node2)->getIndex();
    dsp_stream_set_buffer(nodes->getBaselines()->At(idx)->getStream(), buffer, len);
    dsp_stream_alloc_buffer(nodes->getBaselines()->At(idx)->getStream(), len);
    nodes->getBaselines()->At(idx)->Lock();
}

dsp_stream_p vlbi_get_uv_plot(vlbi_context ctx, int u, int v, double *target, double freq, double sr, int nodelay, int moving_baseline, vlbi_func2_t delegate)
{
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection *baselines = nodes->getBaselines();
    baselines->setWidth(u);
    baselines->setHeight(v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->setRa(target[0]);
    baselines->setDec(target[1]);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    fprintf(stderr, "%d nodes\n%d baselines\n", nodes->Count, baselines->Count);
    fprintf(stderr, "\n");
    baselines->SetDelegate(delegate);
    int num_threads = MAX_THREADS;
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        struct args { VLBIBaseline *b; NodeCollection *nodes; bool moving_baseline; bool nodelay; };
        args argument;
        argument.b = b;
        argument.nodes = nodes;
        argument.moving_baseline = moving_baseline;
        argument.nodelay = nodelay;
        vlbi_start_thread(fillplane, &argument, &parent->child_count, i);
    }
    vlbi_wait_threads(&parent->child_count);
    fprintf(stderr, "\naperture synthesis plotting completed\n");
    return parent;
}

dsp_stream_p vlbi_get_ifft_estimate(dsp_stream_p uv)
{
    dsp_stream_p ifft = dsp_stream_copy(uv);
    dsp_fourier_idft(ifft);
    dsp_buffer_shift(ifft);
    return ifft;
}

dsp_stream_p vlbi_apply_model(dsp_stream_p dft, dsp_stream_p model)
{
    dsp_stream_p uv = dsp_stream_copy(dft);
    dsp_buffer_mul(uv, model->buf, model->len);
    return uv;
}

int vlbi_b64readfile(char *file, void* buf)
{
    FILE *tmp = fopen(file, "r");
    if(tmp != NULL) {
        fseek(tmp, 0, SEEK_END);
        int ilen = ftell(tmp);
        int len = ilen*3/4;
        rewind(tmp);
        char *base64 = (char*)malloc(ilen);
        buf = realloc(buf, len);
        fread(base64, 1, ilen, tmp);
        fclose(tmp);
        from64tobits_fast((char*)buf, (char*)base64, ilen);
        return len;
    }
    return -1;
}
