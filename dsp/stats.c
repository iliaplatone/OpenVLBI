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

double* dsp_stats_histogram(dsp_stream_p stream, int size)
{
    int k;
    double* out = (double*)malloc(sizeof(double)*size);
    long* tmp = (long*)malloc(sizeof(long)*stream->len);
    dsp_buffer_copy(stream->buf, tmp, stream->len);
    dsp_buffer_stretch(tmp, stream->len, 0, size);
    dsp_t mx = 0;
    dsp_t mn = size;
    double  diff = mx - mn;
    diff /= size;
    for(k = 0; k < size; k++) {
        out[k] = (double)dsp_stats_val_count(tmp, stream->len, k);
    }
    free(tmp);
    return out;
}

