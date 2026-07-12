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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nodecollection.h>
#include <baselinecollection.h>
#include <modelcollection.h>
#include <base64.h>
#include <thread>

using namespace std;

extern NodeCollection *vlbi_nodes;

double vlbi_compare_models(vlbi_context ctx, const char *model1, const char *model2)
{
    pfunc;
    double score;
    int dim = 0;
    int rad = 0;
    int num_features = 100;
    int dims;
    dsp_stream_p stream1 = vlbi_get_model(ctx, model1);
    dsp_stream_p stream2 = vlbi_get_model(ctx, model2);
    dims = fmin (stream1->dims, stream2->dims);
    int *firsts = (int*)malloc(sizeof(int)*dims);
    double *factors = (double*)malloc(sizeof(double)*dims);
    for(dim = 0; dim < dims; dim++) {
        double maxsize = fmax(stream1->sizes[dim], stream2->sizes[dim]);
        double minsize = fmin(stream1->sizes[dim], stream2->sizes[dim]);
        firsts[dim] = (minsize == stream1->sizes[dim]);
        stream1->align_info.center[dim] = stream1->sizes[dim] / 2;
        stream2->align_info.center[dim] = stream2->sizes[dim] / 2;
        if(firsts[dim]) {
            factors[dim] = maxsize / minsize;
        }
    }
    stream1->align_info.factor = 0;
    stream2->align_info.factor = 0;
    for(dim = 0; dim < dims; dim++) {
        if(firsts[dim] && factors[dim] > stream1->align_info.factor[dim])
            stream1->align_info.factor[dim] = factors[dim];
        if(!firsts[dim] && factors[dim] > stream2->align_info.factor[dim])
            stream2->align_info.factor[dim] = factors[dim];
    }
    int first = stream1->align_info.factor[dim] < stream2->align_info.factor[dim];
    dsp_stream_p _stream1 = dsp_stream_copy(first ? stream1 : stream2);
    dsp_stream_p _stream2 = dsp_stream_copy(!first ? stream1 : stream2);
    dsp_stream_scale(_stream1);
    for (rad = 0; rad < 128; rad++) {
        _stream1->align_info.radians[dim] = M_PI * 2 * rad / 128.0;
        dsp_stream_rotate(_stream1);
    }
    score = dsp_align_get_offset(_stream1, _stream2, 3, 0.0, num_features);
    dsp_stream_free(_stream1);
    dsp_stream_free(_stream2);
    return score;
}
