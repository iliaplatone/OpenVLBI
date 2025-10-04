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
    int num_features = 100;
    dsp_stream_p stream1 = vlbi_get_model(ctx, model1);
    dsp_stream_p stream2 = vlbi_get_model(ctx, model2);
    score = dsp_align_get_offset(stream1, stream2, 3, 0.0, num_features);
    return score;
}
