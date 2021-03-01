/*
 *   libDSPAU - a digital signal processing library for astronoms usage
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
#include <dsp.h>
#include <time.h>

static int dsp_align_compare_double (const void *arg1, const void *arg2)
{
    double* a1 = (double*)arg1;
    double* a2 = (double*)arg2;
    return ((*a1) < (*a2) ? 1 : -1);
}

int dsp_align_crop_limit(dsp_stream_p reference, dsp_stream_p to_align, int limit, int radius)
{
    limit = Min(reference->stars_count, limit);
    return limit;
}

void *dsp_align_get_offsets_th(void* arg) {
    struct {
        int s;
        dsp_offset* offsets;
        dsp_stream_p stream;
     } *arguments = arg;
    dsp_offset* offsets = arguments->offsets;
    dsp_stream_p stream = arguments->stream;
    int s = arguments->s;
    int l = 0;
}

int dsp_align_get_offset(dsp_stream_p stream1, dsp_stream_p stream2, int max_stars, int decimals)
{
    pfunc;
    int d, x, y, z, xx, yy, zz;
    int matches = 0;
    int num_stars = Min(max_stars, Min(stream1->stars_count, stream2->stars_count));
    int skip_brighter = 0;
    pwarn("stars used=%d\n", num_stars);
    x = y = z = xx = yy = zz = 0;
    decimals = pow(10, decimals);
    double ratio = 0;
    stream2->align_info.factor = 0.0;
    for(d = 0; d < stream2->dims; d++) {
        stream2->align_info.offset[d] = 0;
        stream2->align_info.center[d] = 0;
        if(d > 0)
            stream2->align_info.radians[d-1] = 0;
        ratio += pow(stream2->sizes[d], 2);
    }
    ratio = sqrt(ratio);
    pwarn("creating triangles...\n");
    int err = 7;
    for(x = skip_brighter; x < num_stars; x++) {
        for(y = x+1; y < num_stars; y++) {
            for(z = y+1; z < num_stars; z++) {
                double dist1[3] = { 0, 0, 0 };
                for(d = 0; d < stream2->align_info.dims; d++) {
                    dist1[0] += pow((stream1->stars[y].center.location[d] - stream1->stars[x].center.location[d]), 2);
                    dist1[1] += pow((stream1->stars[z].center.location[d] - stream1->stars[x].center.location[d]), 2);
                    dist1[2] += pow((stream1->stars[z].center.location[d] - stream1->stars[y].center.location[d]), 2);
                }
                dist1[0] = sqrt(dist1[0]);
                dist1[1] = sqrt(dist1[1]);
                dist1[2] = sqrt(dist1[2]);
                qsort(dist1, 3, sizeof(double), dsp_align_compare_double);
                for(xx = skip_brighter; xx < num_stars; xx++) {
                    for(yy = xx+1; yy < num_stars; yy++) {
                        for(zz = yy+1; zz < num_stars; zz++) {
                            stream2->align_info.factor = 0.0;
                            for(d = 0; d < stream2->dims; d++) {
                                stream2->align_info.offset[d] = 0;
                                stream2->align_info.center[d] = 0;
                                if(d < stream2->dims-1)
                                    stream2->align_info.radians[d] = 0.0;
                            }
                            double dist2[3] = { 0, 0, 0 };
                            for(d = 0; d < stream2->align_info.dims; d++) {
                                dist2[0] += pow((stream2->stars[yy].center.location[d] - stream2->stars[xx].center.location[d]), 2);
                                dist2[1] += pow((stream2->stars[zz].center.location[d] - stream2->stars[xx].center.location[d]), 2);
                                dist2[2] += pow((stream2->stars[zz].center.location[d] - stream2->stars[yy].center.location[d]), 2);
                            }
                            dist2[0] = sqrt(dist2[0]);
                            dist2[1] = sqrt(dist2[1]);
                            dist2[2] = sqrt(dist2[2]);
                            qsort(dist2, 3, sizeof(double), dsp_align_compare_double);
                            stream2->align_info.factor = 0.0;
                            for(d = 0; d < stream2->align_info.dims; d++) {
                                double center = stream2->stars[xx].center.location[d];
                                center += stream2->stars[yy].center.location[d];
                                center += stream2->stars[zz].center.location[d];
                                stream2->align_info.center[d] = center / 3;
                                double offset = stream1->stars[x].center.location[d] - stream2->stars[xx].center.location[d];
                                offset += stream1->stars[y].center.location[d] - stream2->stars[yy].center.location[d];
                                offset += stream1->stars[z].center.location[d] - stream2->stars[zz].center.location[d];
                                stream2->align_info.offset[d] = offset / 3;
                            }
                            double _dist1 = 0;
                            double _dist2 = 0;
                            for(d = 0; d < stream2->align_info.dims; d++) {
                                _dist1 += pow((stream1->stars[y].center.location[d] - stream1->stars[x].center.location[d]), 2);
                                _dist2 += pow((stream2->stars[yy].center.location[d] - stream2->stars[xx].center.location[d]), 2);
                            }
                            _dist1 = sqrt(_dist1);
                            _dist2 = sqrt(_dist2);
                            double factor = _dist1 / _dist2;
                            _dist1 = 0;
                            _dist2 = 0;
                            for(d = 0; d < stream2->align_info.dims; d++) {
                                _dist1 += pow((stream1->stars[z].center.location[d] - stream1->stars[x].center.location[d]), 2);
                                _dist2 += pow((stream2->stars[zz].center.location[d] - stream2->stars[xx].center.location[d]), 2);
                            }
                            _dist1 = sqrt(_dist1);
                            _dist2 = sqrt(_dist2);
                            factor += _dist1 / _dist2;
                            _dist1 = 0;
                            _dist2 = 0;
                            for(d = 0; d < stream2->align_info.dims; d++) {
                                _dist1 += pow((stream1->stars[z].center.location[d] - stream1->stars[y].center.location[d]), 2);
                                _dist2 += pow((stream2->stars[zz].center.location[d] - stream2->stars[yy].center.location[d]), 2);
                            }
                            _dist1 = sqrt(_dist1);
                            _dist2 = sqrt(_dist2);
                            factor += _dist1 / _dist2;
                            stream2->align_info.factor += factor / 3;
                            for(d = 0; d < stream2->align_info.dims-1; d++) {
                                stream2->align_info.radians[d] = acos((stream1->stars[y].center.location[d] - stream1->stars[x].center.location[d]) / _dist1);
                                stream2->align_info.radians[d] -= acos((stream2->stars[yy].center.location[d] - stream2->stars[xx].center.location[d]) / _dist2);
                            }
                            for(d = 0; d < stream2->align_info.dims-1; d++) {
                                stream2->align_info.radians[d] += acos((stream1->stars[z].center.location[d] - stream1->stars[x].center.location[d]) / _dist1);
                                stream2->align_info.radians[d] -= acos((stream2->stars[zz].center.location[d] - stream2->stars[xx].center.location[d]) / _dist2);
                            }
                            for(d = 0; d < stream2->align_info.dims-1; d++) {
                                stream2->align_info.radians[d] += acos((stream1->stars[z].center.location[d] - stream1->stars[y].center.location[d]) / _dist1);
                                stream2->align_info.radians[d] -= acos((stream2->stars[zz].center.location[d] - stream2->stars[yy].center.location[d]) / _dist2);
                            }
                            for(d = 0; d < stream2->align_info.dims-1; d++){
                                stream2->align_info.radians[d] /= 3;
                                if(isnan(stream2->align_info.radians[d]))
                                    stream2->align_info.radians[d] = 0.0;
                            }
                            if(fabs(stream2->align_info.offset[0]) < stream2->sizes[0] / decimals &&
                                fabs(stream2->align_info.offset[1]) < stream2->sizes[1] / decimals)
                                err &= ~1;
                            if(round(stream2->align_info.factor * decimals) == decimals)
                                err &= ~2;
                            if(round(stream2->align_info.radians[0] * decimals) == 0.0)
                                err &= ~4;
                            if(!err) {
                                matches++;
                                pwarn("match found!\n");
                                pwarn("reference diameters=%lf, %lf\n", round(stream1->stars[y].diameter*decimals/stream1->stars[x].diameter), round(stream1->stars[z].diameter*decimals/stream1->stars[x].diameter));
                                pwarn("new frame diameters=%lf, %lf\n", round(stream2->stars[yy].diameter*decimals/stream2->stars[xx].diameter), round(stream2->stars[zz].diameter*decimals/stream2->stars[xx].diameter));
                                pwarn("reference sides: %lf, %lf, %lf\n", round(dist1[0]), round(dist1[1]), round(dist1[2]));
                                pwarn("new frame sides: %lf, %lf, %lf\n", round(dist2[0]), round(dist2[1]), round(dist2[2]));
                                pwarn("center coords=x:%d y:%d\n", stream2->align_info.center[0]/matches, stream2->align_info.center[1]/matches);
                                pwarn("center offset=x:%d y:%d\n", stream2->align_info.offset[0]/matches, stream2->align_info.offset[1]/matches);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return err;
}
