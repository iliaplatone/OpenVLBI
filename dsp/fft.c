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
#include <fftw3.h>

double dsp_fourier_complex_get_magnitude(dsp_complex n)
{
    return sqrt (n[0] * n[0] + n[1] * n[1]);
}

double dsp_fourier_complex_get_phase(dsp_complex n)
{
    double out = 0;
    if (n[0] != 0) {
        out = atan (n[1] / n[0]);
    }
    return out;
}

double* dsp_fourier_complex_array_get_magnitude(dsp_complex* in, int len)
{
    int i;
    double* out = (double*)calloc(sizeof(double), len);
    for(i = 0; i < len; i++) {
        out [i] = (double)dsp_fourier_complex_get_magnitude(in [i]);
    }
    return out;
}

double* dsp_fourier_complex_array_get_phase(dsp_complex* in, int len)
{
    int i;
    double* out = (double*)calloc(sizeof(double), len);
    for(i = 0; i < len; i++) {
        out [i] = (double)dsp_fourier_complex_get_phase(in [i]);
    }
    return out;
}

dsp_complex* dsp_fourier_dft(dsp_stream_p stream)
{
    dsp_complex* dft = calloc(sizeof(dsp_complex), stream->len);
    dsp_complex* out = calloc(sizeof(dsp_complex), stream->len);
    fftw_plan plan = fftw_plan_dft(stream->dims, stream->sizes, (fftw_complex*)dft, (fftw_complex*)out, FFTW_FORWARD, FFTW_ESTIMATE);
    for(int x = 0; x < stream->len; x++) {
        dft[x][0] = stream->buf[x];
        dft[x][1] = 0;
    }
    fftw_execute(plan);
    free(plan);
    free(dft);
    return (dsp_complex*)out;
}

dsp_complex* dsp_fourier_idft(dsp_stream_p stream)
{
    dsp_complex* dft = calloc(sizeof(dsp_complex), stream->len);
    dsp_complex* out = calloc(sizeof(dsp_complex), stream->len);
    fftw_plan plan = fftw_plan_dft(stream->dims, stream->sizes, (fftw_complex*)dft, (fftw_complex*)out, FFTW_BACKWARD, FFTW_ESTIMATE);
    for(int x = 0; x < stream->len; x++) {
        dft[x][0] = stream->buf[x];
        dft[x][1] = 0;
    }
    fftw_execute(plan);
    free(plan);
    free(dft);
    return (dsp_complex*)out;
}

void dsp_fourier_dft_magnitude(dsp_stream_p stream)
{
    dsp_complex* dft = dsp_fourier_dft(stream);
    double* mag = dsp_fourier_complex_array_get_magnitude(dft, stream->len);
    free(dft);
    double mn = dsp_stats_min(mag, stream->len);
    double mx = dsp_stats_max(mag, stream->len);
    dsp_buffer_stretch(mag, stream->len, mn, mx);
    dsp_buffer_copy(mag, stream->buf, stream->len);
    free(mag);
}

void dsp_fourier_dft_phase(dsp_stream_p stream)
{
    dsp_complex* dft = dsp_fourier_dft(stream);
    double* phi = dsp_fourier_complex_array_get_phase(dft, stream->len);
    free(dft);
    double mn = dsp_stats_min(phi, stream->len);
    double mx = dsp_stats_max(phi, stream->len);
    dsp_buffer_stretch(phi, stream->len, mn, mx);
    dsp_buffer_copy(phi, stream->buf, stream->len);
    free(phi);
}

void dsp_fourier_idft_magnitude(dsp_stream_p stream)
{
    dsp_complex* dft = dsp_fourier_idft(stream);
    double* mag = dsp_fourier_complex_array_get_magnitude(dft, stream->len);
    free(dft);
    double mn = dsp_stats_min(mag, stream->len);
    double mx = dsp_stats_max(mag, stream->len);
    dsp_buffer_stretch(mag, stream->len, mn, mx);
    dsp_buffer_copy(mag, stream->buf, stream->len);
    free(mag);
}

void dsp_fourier_idft_phase(dsp_stream_p stream)
{
    dsp_complex* dft = dsp_fourier_idft(stream);
    double* phi = dsp_fourier_complex_array_get_phase(dft, stream->len);
    free(dft);
    double mn = dsp_stats_min(phi, stream->len);
    double mx = dsp_stats_max(phi, stream->len);
    dsp_buffer_stretch(phi, stream->len, mn, mx);
    dsp_buffer_copy(phi, stream->buf, stream->len);
    free(phi);
}
