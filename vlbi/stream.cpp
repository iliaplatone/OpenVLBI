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
	int nt = 0;
        for (unsigned long int t = 0; t < vlbi_max_threads(0); t++)
		nt += (((int)*thread_cnt)&(1 << t))>>t;
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

static void* fillplane_aperture_synthesis(void* arg)
{
    VLBIBaseline *b = (VLBIBaseline*)arg;
    dsp_stream_p s = b->getStream();
    dsp_stream_p parent = (dsp_stream_p)s->parent;
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    double tao = 1.0 / parent->samplerate;
    double st = b->getStartTime();
    double et = st + s->len * tao;
    for(double time = st; time < et; time += tao) {
        double *uvcoords = b->getProjection();
        if(uvcoords == NULL)
            continue;
        int U = uvcoords[0] + u / 2;
        int V = uvcoords[1] + v / 2;
        free(uvcoords);
        if(U >= 0 && U < u && V >= 0 && V < v) {
            int idx = U+V*u;
            double val = b->Correlate(time);
            parent->buf[idx] = val;
            parent->buf[parent->len-idx] = val;
        }
        fprintf(stderr, "\r%.3f%%   ", (time-st)*100.0/(et-st-tao));
    }
    return NULL;
}

static void* fillplane_moving_baseline(void* arg)
{
    VLBIBaseline *b = (VLBIBaseline*)arg;
    dsp_stream_p s = b->getStream();
    dsp_stream_p parent = (dsp_stream_p)s->parent;
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    for(int i = 0; i < s->len; i++) {
        double *uvcoords = b->getProjection();
        if(uvcoords == NULL)
            continue;
        int U = uvcoords[0] + u / 2;
        int V = uvcoords[1] + v / 2;
        free(uvcoords);
        if(U >= 0 && U < u && V >= 0 && V < v) {
            int idx = U+V*u;
            double val = b->Correlate(i);
            parent->buf[idx] = val;
            parent->buf[parent->len-idx] = val;
        }
        fprintf(stderr, "\r%.3f%%   ", i*100.0/s->len);
    }
    return NULL;
}

static void* fillplane_coverage(void* arg)
{
    VLBIBaseline *b = (VLBIBaseline*)arg;
    dsp_stream_p s = b->getStream();
    dsp_stream_p parent = (dsp_stream_p)s->parent;
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    double tao = 1.0 / parent->samplerate;
    double st = b->getStartTime();
    double et = st + s->len * tao;
    for(double time = st; time < et; time += tao) {
        double *uvcoords = b->getProjection();
        if(uvcoords == NULL)
            continue;
        int U = uvcoords[0] + u / 2;
        int V = uvcoords[1] + v / 2;
        free(uvcoords);
        if(U >= 0 && U < u && V >= 0 && V < v) {
            int idx = U+V*u;
            parent->buf[idx] = 1;
            parent->buf[parent->len-idx] = 1;
        }
        fprintf(stderr, "\r%.3f%%   ", (time-st)*100.0/(et-st-tao));
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

void vlbi_add_stream(void *ctx, dsp_stream_p Stream, char* name, int geo) {
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p stream = dsp_stream_copy(Stream);
    int na = ('Z' - 'A');
    stream->arg = calloc(150, 1);
    sprintf((char*)stream->arg, "%c%c", (nodes->Count % na) + 'A', ((nodes->Count / na) % 10) + '0');
    nodes->Add(new VLBINode(stream, geo), name);
    nodes->At(nodes->Count-1)->setLocation(Stream->location[0], Stream->location[1], Stream->location[2]);
}

void vlbi_del_stream(void *ctx, char* name) {
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    nodes->RemoveKey(name);
}

dsp_stream_p vlbi_get_uv_plot_aperture_synthesis(vlbi_context ctx, int u, int v, double *target, double freq, double sr)
{
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection *baselines = new BaselineCollection(nodes, u, v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->SetTarget(target);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    fprintf(stderr, "%d nodes\n%d baselines\n", nodes->Count, baselines->Count);
    fprintf(stderr, "\n");
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        vlbi_start_thread(fillplane_aperture_synthesis, b, &parent->child_count, i);
    }
    vlbi_wait_threads(&parent->child_count);
    fprintf(stderr, "\nearth tide plotting completed\n");
    return parent;
}

dsp_stream_p vlbi_get_uv_plot_moving_baseline(void *ctx, int u, int v, double *target, double freq, double sr)
{
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection *baselines = new BaselineCollection(nodes, u, v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->SetTarget(target);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    fprintf(stderr, "%d nodes\n%d baselines\n", nodes->Count, baselines->Count);
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        vlbi_start_thread(fillplane_moving_baseline, b, &parent->child_count, i);
    }
    vlbi_wait_threads(&parent->child_count);
    fprintf(stderr, "\nmoving baselines plotting completed\n");
    return parent;
}

dsp_stream_p vlbi_get_uv_plot_coverage(void *ctx, int u, int v, double *target, double freq, double sr)
{
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection *baselines = new BaselineCollection(nodes, u, v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->SetTarget(target);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    fprintf(stderr, "%d nodes\n%d baselines\n", nodes->Count, baselines->Count);
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        vlbi_start_thread(fillplane_coverage, b, &parent->child_count, i);
    }
    vlbi_wait_threads(&parent->child_count);
    fprintf(stderr, "\ncoverage plotting completed\n");
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
