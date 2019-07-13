#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <linux/fcntl.h>
#include <string.h>
#include <pthread.h>
#include <nodecollection.h>
#include <baselinecollection.h>

static NodeCollection *vlbi_nodes = new NodeCollection();

typedef struct _vlbi_thread_t {
    void *(*__start_routine) (void *);
    void* arg;
    int* thread_cnt;
    pthread_t th;
}vlbi_thread_t;

char* vlbi_get_version() { return VLBI_VERSION_STRING; }

static void vlbi_wait_threads(int n, int *thread_cnt)
{
    int nt = *thread_cnt;
    while(nt > n) {
        nt = *thread_cnt;
        fprintf(stderr, "running threads: %d\n", nt);
        usleep(100000);
    }
}

static void* vlbi_thread_func(void* arg)
{
    vlbi_thread_t *t = (vlbi_thread_t*)arg;
    t->__start_routine(t->arg);
    int nt = *t->thread_cnt;
    *t->thread_cnt = nt - 1;
    free(t);
    return NULL;
}

static void vlbi_start_thread(void *(*__start_routine) (void *), void *arg, int* thread_cnt)
{
    int nt = *thread_cnt;
    *thread_cnt = nt + 1;
    vlbi_thread_t *t = (vlbi_thread_t *)malloc(sizeof(vlbi_thread_t));
    t->arg = arg;
    t->thread_cnt = thread_cnt;
    t->__start_routine = __start_routine;
    pthread_create(&t->th, NULL, vlbi_thread_func, t);
}

static void* correlate_astro(void* arg)
{
    VLBIBaseline *b = (VLBIBaseline*)arg;
    dsp_stream_p s = b->getStream();
    dsp_stream_p parent = (dsp_stream_p)s->parent;
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    double tao = 1.0 / parent->samplerate;
    double st = vlbi_time_timespec_to_J2000time(s->starttimeutc);
    double et = st + s->len * tao;
    for(double time = st; time < et; time += tao) {
        double *uvcoords = b->getUVCoords(time);
        if(uvcoords == NULL)
            continue;
        int U = (int)uvcoords[0];
        int V = (int)uvcoords[1];
        free(uvcoords);
        U += u / 2;
        V += v / 2;
        if(U >= 0 && U < u && V >= 0 && V < v) {
            int idx = (int)(U + V * u);
            dsp_t* c = b->Correlate(time);
            parent->buf[idx] += *c;
            parent->buf[parent->len - idx - 1] += *c;
        }
    }

   return NULL;
}

static void* correlate_moving_baseline(void* arg)
{
    VLBIBaseline *b = (VLBIBaseline*)arg;
    dsp_stream_p s = b->getStream();
    dsp_stream_p parent = (dsp_stream_p)s->parent;
    int u = parent->sizes[0];
    int v = parent->sizes[1];
    for(double i = 0; i < s->len; i++) {
        double *uvcoords = b->getUVCoords(i);
        if(uvcoords == NULL)
            continue;
        int U = (int)uvcoords[0];
        int V = (int)uvcoords[1];
        free(uvcoords);
        U += u / 2;
        V += v / 2;
        if(U >= 0 && U < u && V >= 0 && V < v) {
            int idx = (int)(U + V * u);
            dsp_t* c = b->Correlate(i);
            parent->buf[idx] += *c;
            parent->buf[parent->len - idx - 1] += *c;
        }
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

void vlbi_add_stream(void *ctx, dsp_stream_p Stream) {
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    dsp_stream_p stream = dsp_stream_copy(Stream);
    int na = ('Z' - 'A');
    stream->arg = calloc(150, 1);
    sprintf((char*)stream->arg, "%c%c", (nodes->Count % na) + 'A', ((nodes->Count / na) % 10) + '0');
    nodes->Add(new VLBINode(stream));
}

dsp_stream_p vlbi_get_uv_plot_earth_tide(vlbi_context ctx, vlbi_func2_t correlation_func, int m, int u, int v, double *target, double freq, double sr)
{
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection *baselines = new BaselineCollection(nodes, correlation_func, m, u, v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->SetTarget(target);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        vlbi_start_thread(correlate_astro, b, &parent->child_count);
    }
    vlbi_wait_threads(0, &parent->child_count);
    return parent;
}

dsp_stream_p vlbi_get_uv_plot_moving_baseline(void *ctx, vlbi_func2_t correlation_func, int m, int u, int v, double *target, double freq, double sr)
{
    NodeCollection *nodes = (ctx != NULL) ? (NodeCollection*)ctx : vlbi_nodes;
    BaselineCollection *baselines = new BaselineCollection(nodes, correlation_func, m, u, v);
    baselines->SetFrequency(freq);
    baselines->SetSampleRate(sr);
    baselines->SetTarget(target);
    dsp_stream_p parent = baselines->getStream();
    parent->child_count = 0;
    for(int i = 0; i < baselines->Count; i++)
    {
        VLBIBaseline *b = baselines->At(i);
        vlbi_start_thread(correlate_moving_baseline, b, &parent->child_count);
    }
    vlbi_wait_threads(0, &parent->child_count);
    return parent;
}

dsp_stream_p vlbi_get_fft_estimate(dsp_stream_p uv)
{
    dsp_stream_p fft = dsp_stream_copy(uv);
    dsp_buffer_stretch(fft->buf, fft->len, 0.0, 1.0);
    dsp_fourier_dft_magnitude(fft);
    dsp_buffer_shift(fft);
    return fft;
}

dsp_stream_p vlbi_apply_model(dsp_stream_p uv, dsp_stream_p model)
{
    dsp_stream_p fft = dsp_stream_copy(uv);
    dsp_fourier_dft_magnitude(model);
    dsp_buffer_shift(model);
    for(int i= 0; i < model->len; i++) {
        if(uv->buf[i] == 0)
            model->buf[i] = 0;
        else
            fft->buf[i] *= model->buf[i];
    }
    dsp_buffer_stretch(fft->buf, fft->len, 0.0, 1.0);
    return fft;
}
