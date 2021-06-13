/*
 *   libDSPAU - a digital signal processing library for astronomy usage
 *   Copyright (C) 2017  Ilia Platone <info@iliaplatone.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsp.h"
#include <setjmp.h>
#include <signal.h>

jmp_buf jump;

static void segv (int sig)
{
  longjmp (jump, 1);
}

int dsp_buffer_check(void *x)
{
  volatile char c;
  int illegal = 0;

  signal (SIGSEGV, segv);

  if (!setjmp (jump))
    c = *(char *) (x);
  else
    illegal = 1;

  signal (SIGSEGV, SIG_DFL);

  return illegal;
}

void dsp_buffer_shift(dsp_stream_p stream)
{
    if(stream->dims == 0)
        return;
    dsp_t* tmp = (dsp_t*)malloc(sizeof(dsp_t) * stream->len);
    int x, d;
    for(x = 0; x < stream->len/2; x++) {
        int* pos = dsp_stream_get_position(stream, x);
        for(d = 0; d < stream->dims; d++) {
            if(pos[d]<stream->sizes[d] / 2) {
                pos[d] += stream->sizes[d] / 2;
            } else {
                pos[d] -= stream->sizes[d] / 2;
            }
        }
        tmp[x] = stream->buf[dsp_stream_set_position(stream, pos)];
        tmp[dsp_stream_set_position(stream, pos)] = stream->buf[x];
        free(pos);
    }
    memcpy(stream->buf, tmp, stream->len * sizeof(dsp_t));
    free(tmp);
}

void dsp_buffer_removemean(dsp_stream_p stream)
{
    int k;

    dsp_t mean = dsp_stats_mean(stream->buf, stream->len);
    for(k = 0; k < stream->len; k++)
        stream->buf[k] = stream->buf[k] - mean;

}

void dsp_buffer_sub(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

	int k;
	for(k = 0; k < len; k++) {
        stream->buf[k] = stream->buf[k] - in[k];
	}

}

void dsp_buffer_sum(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

    int k;
    for(k = 0; k < len; k++) {
        stream->buf[k] += in[k];
    }

}

void dsp_buffer_max(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

    int k;
    for(k = 0; k < len; k++) {
        stream->buf[k] = Max(stream->buf[k], in[k]);
    }

}

void dsp_buffer_min(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

    int k;
    for(k = 0; k < len; k++) {
        stream->buf[k] = Min(stream->buf[k], in[k]);
    }

}

void dsp_buffer_div(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

	int k;
	for(k = 0; k < len; k++) {
        stream->buf[k] = stream->buf[k] / in[k];
	}

}

void dsp_buffer_mul(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

    int k;
    for(k = 0; k < len; k++) {
        stream->buf[k] = stream->buf[k] * in[k];
    }

}

void dsp_buffer_pow(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

    int k;
    for(k = 0; k < len; k++) {
        stream->buf[k] = pow(stream->buf[k], in[k]);
    }

}

void dsp_buffer_log(dsp_stream_p stream, dsp_t* in, int inlen)
{
    int len = Min(stream->len, inlen);

    int k;
    for(k = 0; k < len; k++) {
        stream->buf[k] = Log(stream->buf[k], in[k]);
    }

}

void dsp_buffer_1sub(dsp_stream_p stream, dsp_t val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] = val - stream->buf[k];
    }

}

void dsp_buffer_sub1(dsp_stream_p stream, dsp_t val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] = stream->buf[k] - val;
    }

}

void dsp_buffer_sum1(dsp_stream_p stream, dsp_t val)
{
	int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] += val;
	}

}

void dsp_buffer_1div(dsp_stream_p stream, double val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] = val / stream->buf[k];
    }

}

void dsp_buffer_div1(dsp_stream_p stream, double val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] /= val;
    }

}

void dsp_buffer_mul1(dsp_stream_p stream, double val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] = stream->buf[k] * val;
    }

}

void dsp_buffer_pow1(dsp_stream_p stream, double val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] = pow(stream->buf[k], val);
    }

}

void dsp_buffer_log1(dsp_stream_p stream, double val)
{
    int k;

    for(k = 0; k < stream->len; k++) {
        stream->buf[k] = Log(stream->buf[k], val);
    }

}

static int compare( const void* a, const void* b)
{
     dsp_t int_a = * ( (dsp_t*) a );
     dsp_t int_b = * ( (dsp_t*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

static void* dsp_buffer_median_th(void* arg)
{
    struct {
        int cur_th;
        int size;
        int median;
        dsp_stream_p stream;
     } *arguments = arg;
    dsp_stream_p stream = arguments->stream;
    dsp_stream_p in = stream->parent;
    int cur_th = arguments->cur_th;
    int size = arguments->size;
    int median = arguments->median;
    int start = cur_th * stream->len / DSP_MAX_THREADS;
    int end = start + stream->len / DSP_MAX_THREADS;
    end = Min(stream->len, end);
    int x, y, dim, idx;
    dsp_t* sorted = (dsp_t*)malloc(pow(size, stream->dims) * sizeof(dsp_t));
    for(x = start; x < end; x++) {
        int *pos = dsp_stream_get_position(stream, x);
        dsp_t* buf = sorted;
        for(dim = 0; dim < stream->dims; dim++) {
            pos[dim] -= size / 2;
            for(y = 0; y < size; y++, pos[dim]++) {
                idx = dsp_stream_set_position(stream, pos);
                if(idx >= 0 && idx < in->len)
                    *buf++ = in->buf[idx];
            }
        }
        qsort(sorted, size, sizeof(dsp_t), compare);
        stream->buf[x] = sorted[median];
        free(pos);
    }
    free(sorted);
}

void dsp_buffer_median(dsp_stream_p in, int size, int median)
{
    start_gettime;
    int dims = in->dims;
    int dim, y;
    dsp_stream_p stream = dsp_stream_copy(in);
    dsp_buffer_set(stream->buf, stream->len, 0);
    stream->parent = in;
    pthread_t *th = malloc(sizeof(pthread_t)*DSP_MAX_THREADS);
    struct {
       int cur_th;
       int size;
       int median;
       dsp_stream_p stream;
    } thread_arguments[DSP_MAX_THREADS];
    for(y = 0; y < DSP_MAX_THREADS; y++)
    {
        thread_arguments[y].cur_th = y;
        thread_arguments[y].size = size;
        thread_arguments[y].median = median;
        thread_arguments[y].stream = stream;
        pthread_create(&th[y], NULL, dsp_buffer_median_th, &thread_arguments[y]);
    }
    for(y = 0; y < DSP_MAX_THREADS; y++)
        pthread_join(th[y], NULL);
    free(th);
    stream->parent = NULL;
    dsp_buffer_copy(stream->buf, in->buf, stream->len);
    dsp_stream_free_buffer(stream);
    dsp_stream_free(stream);
    end_gettime;
}

void dsp_buffer_deviate(dsp_stream_p stream, dsp_t* deviation, dsp_t mindeviation, dsp_t maxdeviation)
{
    dsp_stream_p tmp = dsp_stream_copy(stream);
    int k;
    for(k = 1; k < stream->len; k++) {
        stream->buf[(int)Max(0, Min(stream->len, ((deviation[k] - mindeviation) * (maxdeviation - mindeviation) + mindeviation) + k))] = tmp->buf[k];
    }
    dsp_stream_free(tmp);
}
