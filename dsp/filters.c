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

void dsp_filter_squarelaw(dsp_stream_p stream)
{
    dsp_t* in = stream->buf;
    dsp_t *out = (dsp_t*)malloc(sizeof(dsp_t) * stream->len);
    int len = stream->len;
    dsp_t mean = dsp_stats_mean(stream->buf, stream->len);
    int val = 0;
    int i;
    for(i = 0; i < len; i++) {
        val = in [i] - mean;
        out [i] = (abs (val) + mean);
    }
    memcpy(stream->buf, out, stream->len * sizeof(dsp_t));
    free(out);
}

void dsp_filter_lowpass(dsp_stream_p stream, double Frequency)
{
    int d, x;
    double radius = 0.0;
    for(d = 0; d < stream->dims; d++) {
        radius += pow(stream->sizes[d]/2.0, 2);
    }
    radius = sqrt(radius);
    dsp_stream_p matrix = dsp_stream_copy(stream);
    dsp_buffer_set(matrix->buf, matrix->len, 0);
    for(x = 0; x < matrix->len; x++) {
        int* pos = dsp_stream_get_position(matrix, x);
        double dist = 0.0;
        for(d = 0; d < matrix->dims; d++) {
            dist += pow(matrix->sizes[d]/2.0-pos[d], 2);
        }
        free(pos);
        dist = fmax(0.0, fmin(M_PI, sqrt(dist)*M_PI*Frequency/radius));
        matrix->buf[x] = (cos(dist)+1.0) * dsp_t_max / 2.0;
    }
    dsp_convolution_convolution(stream, matrix);
    dsp_stream_free_buffer(matrix);
    dsp_stream_free(matrix);
}

void dsp_filter_highpass(dsp_stream_p stream, double Frequency)
{
    dsp_stream_p lowpass = dsp_stream_copy(stream);
    dsp_filter_lowpass(lowpass, Frequency);
    dsp_buffer_sub(stream, lowpass->buf, lowpass->len);
    dsp_stream_free_buffer(lowpass);
    dsp_stream_free(lowpass);
}

void dsp_filter_bandreject(dsp_stream_p stream, double LowFrequency, double HighFrequency)
{
    dsp_stream_p lowpass = dsp_stream_copy(stream);
    dsp_stream_p hipass = dsp_stream_copy(stream);
    dsp_filter_lowpass(lowpass, LowFrequency);
    dsp_filter_highpass(hipass, HighFrequency);
    dsp_buffer_copy(lowpass->buf, stream->buf, stream->len);
    dsp_buffer_div1(stream, 2.0);
    dsp_buffer_sum(stream, hipass->buf, stream->len);
    dsp_stream_free_buffer(lowpass);
    dsp_stream_free(lowpass);
    dsp_stream_free_buffer(hipass);
    dsp_stream_free(hipass);
}

void dsp_filter_bandpass(dsp_stream_p stream, double LowFrequency, double HighFrequency)
{
    dsp_stream_p lowpass = dsp_stream_copy(stream);
    dsp_stream_p hipass = dsp_stream_copy(stream);
    dsp_filter_lowpass(lowpass, HighFrequency);
    dsp_filter_highpass(lowpass, LowFrequency);
    dsp_buffer_copy(lowpass->buf, stream->buf, stream->len);
    dsp_stream_free_buffer(lowpass);
    dsp_stream_free(lowpass);
    dsp_stream_free_buffer(hipass);
    dsp_stream_free(hipass);
}
