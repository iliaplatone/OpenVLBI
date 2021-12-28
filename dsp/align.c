/*
*   DSP API - a digital signal processing library for astronomy usage
*   Copyright © 2017-2021  Ilia Platone
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

#include "dsp.h"
#include <time.h>

int dsp_qsort_double_desc (const void *arg1, const void *arg2)
{
    double* a1 = (double*)arg1;
    double* a2 = (double*)arg2;
    return ((*a1) < (*a2) ? 1 : -1);
}

int dsp_qsort_double_asc (const void *arg1, const void *arg2)
{
    double* a1 = (double*)arg1;
    double* a2 = (double*)arg2;
    return ((*a1) > (*a2) ? 1 : -1);
}

int dsp_qsort_star_diameter_desc(const void *arg1, const void *arg2)
{
    dsp_star* a = (dsp_star*)arg1;
    dsp_star* b = (dsp_star*)arg2;
    if(a->diameter < b->diameter)
        return 1;
    return -1;
}

int dsp_qsort_star_diameter_asc(const void *arg1, const void *arg2)
{
    dsp_star* a = (dsp_star*)arg1;
    dsp_star* b = (dsp_star*)arg2;
    if(a->diameter > b->diameter)
        return 1;
    return -1;
}

static double match_err(dsp_triangle t1, dsp_triangle t2, dsp_align_info align_info)
{
    double size1 = t1.sizes[0];
    double size2 = t2.sizes[0]/align_info.factor[0];
    double x1 = (t1.stars[1].center.location[0]-t1.stars[0].center.location[0]);
    double y1 = (t1.stars[1].center.location[1]-t1.stars[0].center.location[1]);
    double _x2 = (t2.stars[1].center.location[0]-align_info.offset[0]-align_info.center[0]);
    double _y2 = (t2.stars[1].center.location[1]-align_info.offset[1]-align_info.center[1]);
    double x2 = _x2*cos(align_info.radians[0])+_y2*sin(align_info.radians[0]);
    double y2 = _y2*cos(align_info.radians[0])-_x2*sin(align_info.radians[0]);
    x2 /= align_info.factor[0];
    y2 /= align_info.factor[1];
    return fabs(((x2-x1)+(y2-y1)+(size2-size1))/size1);
}

static double calc_match_score(dsp_triangle t1, dsp_triangle t2, dsp_align_info align_info)
{
    double size1 = t1.sizes[0];
    double size2 = t2.sizes[0]/align_info.factor[0];
    double radians = align_info.radians[0];
    if (radians > M_PI)
        radians -= M_PI*2.0;
    double score = fabs((t1.index - t2.index)/t1.index);
    score += fabs((t1.ratios[1] - t2.ratios[1])/t1.ratios[1]);
    score += fabs((t1.ratios[2] - t2.ratios[2])/t1.ratios[2]);
    score += fabs((size1 - size2)/size1);
    score += match_err(t1, t2, align_info);
    score /= 5.0;
    return score;
}

static dsp_align_info fill_align_info(dsp_triangle t1, dsp_triangle t2)
{
    dsp_align_info align_info;
    int dims = t1.dims - 1;
    int d;
    align_info.dims = dims;
    align_info.center = (double*)malloc(sizeof(double)*(dims));
    align_info.offset = (double*)malloc(sizeof(double)*(dims));
    align_info.radians = (double*)malloc(sizeof(double)*(dims-1));
    align_info.factor = (double*)malloc(sizeof(double)*(dims));
    for(d = 0; d < dims; d++) {
        align_info.offset[d] = 0;
        align_info.center[d] = 0;
        if(d > 0)
            align_info.radians[d-1] = 0;
    }
    for(d = 0; d < dims; d++) {
        align_info.center[d] = t2.stars[0].center.location[d];
        align_info.offset[d] = t2.stars[0].center.location[d] - t1.stars[0].center.location[d];
    }
    for(d = 0; d < t2.dims; d++)
        align_info.factor[d] += t2.sizes[d] / t1.sizes[d];
    align_info.radians[0] = t1.theta - t2.theta;
    if(align_info.radians[0] >= M_PI*2.0)
        align_info.radians[0] -= M_PI*2.0;
    if(align_info.radians[0] < 0.0)
        align_info.radians[0] += M_PI*2.0;
    align_info.score = calc_match_score(t1, t2, align_info);
    return align_info;
}

static dsp_triangle calc_triangle(dsp_star* stars)
{
    dsp_triangle triangle;
    triangle.dims = stars[0].center.dims+1;
    triangle.sizes = (double*)malloc(sizeof(double)*triangle.dims);
    triangle.stars = (dsp_star*)malloc(sizeof(dsp_star)*triangle.dims);
    for(int d = 0; d < triangle.dims; d++) {
        triangle.stars[d].center.dims = stars[0].center.dims;
        triangle.stars[d].center.location = (double*)malloc(sizeof(double)*stars[0].center.dims);
    }
    double delta[3];
    int d;
    double diff[3][2];
    qsort(stars, 3, sizeof(dsp_star), dsp_qsort_star_diameter_desc);
    diff[0][0] = stars[0].center.location[0]-stars[1].center.location[0];
    diff[1][0] = stars[1].center.location[0]-stars[2].center.location[0];
    diff[2][0] = stars[0].center.location[0]-stars[2].center.location[0];
    diff[0][1] = stars[0].center.location[1]-stars[1].center.location[1];
    diff[1][1] = stars[1].center.location[1]-stars[2].center.location[1];
    diff[2][1] = stars[0].center.location[1]-stars[2].center.location[1];
    delta[0] = sqrt(pow(diff[0][0], 2)+pow(diff[0][1], 2));
    delta[1] = sqrt(pow(diff[1][0], 2)+pow(diff[1][1], 2));
    delta[2] = sqrt(pow(diff[2][0], 2)+pow(diff[2][1], 2));
    triangle.theta = acos(diff[0][0] / delta[0]);
    if(diff[0][1] < 0)
        triangle.theta = -triangle.theta;
    if(triangle.theta >= M_PI*2.0)
        triangle.theta -= M_PI*2.0;
    if(triangle.theta < 0.0)
        triangle.theta += M_PI*2.0;
    double index = acos(diff[1][0] / delta[1]);
    if(diff[1][1] < 0)
        index = -index;
    index -= triangle.theta;
    triangle.index = index;
    index = acos(diff[2][0] / delta[2]);
    if(diff[2][1] < 0)
        index = -index;
    index -= triangle.theta;
    triangle.index += index;
    if(triangle.index >= M_PI)
        triangle.index -= M_PI*2.0;
    if(triangle.index < -M_PI)
        triangle.index += M_PI*2.0;
    qsort(delta, 3, sizeof(double), dsp_qsort_double_desc);
    triangle.sizes[0] = delta[0];
    triangle.sizes[1] = delta[1];
    triangle.sizes[2] = delta[2];
    triangle.stars[0].diameter = stars[0].diameter;
    triangle.stars[1].diameter = stars[1].diameter;
    triangle.stars[2].diameter = stars[2].diameter;
    triangle.stars[0].center.dims = stars[0].center.dims;
    triangle.stars[1].center.dims = stars[1].center.dims;
    triangle.stars[2].center.dims = stars[2].center.dims;
    for(d = 0; d < stars[0].center.dims; d++) {
        triangle.stars[0].center.location[d] = stars[0].center.location[d];
        triangle.stars[1].center.location[d] = stars[1].center.location[d];
        triangle.stars[2].center.location[d] = stars[2].center.location[d];
    }
    return triangle;
}

int dsp_align_get_offset(dsp_stream_p stream1, dsp_stream_p stream2, double tolerance, double target_score)
{
    double decimals = pow(10, tolerance);
    double ratio = decimals*1600.0/sqrt(pow(stream2->sizes[0], 2)+pow(stream2->sizes[1], 2));
    pwarn("decimals: %lf\n", decimals);
    target_score = 1.0-target_score/100.0;
    double min_score = 1.0;
    stream2->align_info.err = 0xf;
    stream2->align_info.dims = 2;
    stream2->align_info.score = 1.0;
    stream2->align_info.decimals = decimals;
    stream2->align_info.triangles_count = 0;
    int t1, t2, x, y, z, s1, s2;
    x=y=0;
    s1 = 0;
    s2 = 0;
    pgarb("creating triangles for reference frame...\n");
    stream1->triangles_count = 0;
    for(x = s1; x < stream1->stars_count; x++) {
        for(y = x+1; y < stream1->stars_count; y++) {
            for(z = y+1; z < stream1->stars_count; z++) {
                dsp_stream_add_triangle(stream1, calc_triangle((dsp_star[3]){stream1->stars[x], stream1->stars[y], stream1->stars[z]}));
            }
        }
    }
    pgarb("creating triangles for current frame...\n");
    stream2->triangles_count = 0;
    for(x = s2; x < stream2->stars_count; x++) {
        for(y = x+1; y < stream2->stars_count; y++) {
            for(z = y+1; z < stream2->stars_count; z++) {
                dsp_stream_add_triangle(stream2, calc_triangle((dsp_star[3]){stream2->stars[x], stream2->stars[y], stream2->stars[z]}));
            }
        }
    }
    dsp_align_info align_info;
    for(t1 = 0; t1 < stream1->triangles_count; t1++) {
        for(t2 = 0; t2 < stream2->triangles_count; t2++) {
            align_info = fill_align_info(stream1->triangles[t1], stream2->triangles[t2]);
            if(align_info.score < min_score) {
                stream2->align_info = align_info;
                min_score = align_info.score;
            }
        }
    }
    if(min_score < target_score)
        stream2->align_info.err &= ~DSP_ALIGN_NO_MATCH;
    if(fabs(sqrt(pow(stream2->align_info.offset[0], 2)+pow(stream2->align_info.offset[1], 2))) * ratio * 10.0 < 1)
        stream2->align_info.err &= ~DSP_ALIGN_TRANSLATED;
    if(fabs((stream2->align_info.factor[0] - 1.0)) * decimals * 10.0 < 1)
        stream2->align_info.err &= ~DSP_ALIGN_SCALED;
    double radians = stream2->align_info.radians[0];
    if(radians > M_PI)
        radians -= M_PI*2;
    if(fabs(radians) * decimals * 10.0 < 1)
        stream2->align_info.err &= ~DSP_ALIGN_ROTATED;
    return stream2->align_info.err;
}
