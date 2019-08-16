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

#include "baseline.h"

VLBIBaseline::VLBIBaseline(dsp_stream_p node1, dsp_stream_p node2, bool m)
{
    Stream = dsp_stream_new();
    Stream->arg = calloc(150, 1);
    sprintf((char*)Stream->arg, "%s_%s", (char*)node1->arg, (char*)node2->arg);
    double starttime1 = vlbi_time_timespec_to_J2000time(node1->starttimeutc);
    double starttime2 = vlbi_time_timespec_to_J2000time(node2->starttimeutc);
    starttime = Max(starttime1, starttime2);
    Stream->starttimeutc = vlbi_time_J2000time_to_timespec(starttime);
    Stream->len = Max(node1->len, node2->len);
    deltatime = 0;
    if(starttime1 < starttime2) {
        first = node1;
        second = node2;
    } else {
        first = node2;
        second = node1;
    }
<<<<<<< HEAD
    dsp_fourier_dft_magnitude(first);
    dsp_fourier_dft_magnitude(second);
    dsp_buffer_reverse(second->buf, second->len);
    deltatime = fabs(starttime1 - starttime2);
    Name = (char*)Stream->arg;
}

double *VLBIBaseline::Correlate(double* buf, double J2000_Offset_Time, int *idx)
{
    double* projection = getUVCoords(J2000_Offset_Time);
    double delay = vlbi_distance_delay(projection[2]);
    int i = (int)(((J2000_Offset_Time - starttime) + deltatime + delay) * first->samplerate);
    dsp_stream_p s1 = dsp_stream_copy(first);
    s1->buf = &s1->buf[i];
    s1->len = second->len-i;
    dsp_convolution_convolution(s1, second);
    dsp_buffer_copy(s1->buf, buf, (second->len-i));
    dsp_stream_free(s1);
    *idx = i;
=======
    timediff = abs(starttime1 - starttime2);
    if(!m)
        baseline_m = vlbi_calc_baseline_m(node1->location, node2->location);
    else
        baseline_m = vlbi_calc_baseline_m_xyz(node1->location, node2->location);
    baseline_center = vlbi_calc_baseline_center(node1->location, node2->location);
    baseline_rad = vlbi_calc_baseline_rad(node1->location, node2->location);
    Name = (char*)Stream->arg;
}

double *VLBIBaseline::Correlate(double* buf, double J2000Time, int *idx)
{
    double delay = vlbi_calc_baseline_delay(first->location, second->location, Stream->target, J2000Time);
    int i = (int)((J2000Time - starttime) + delay * first->samplerate);
    *idx = i;
    if(i<=0)
       return NULL;
    dsp_stream_p s2 = dsp_stream_copy(second);
    dsp_stream_p s1 = dsp_stream_new();
    dsp_stream_add_dim(s1, first->len-i);
    dsp_stream_alloc_buffer(s1, s1->len);
    dsp_buffer_copy(((double*)&first->buf[i]), s1->buf, s1->len);
    dsp_fourier_dft_magnitude(s1);
    dsp_fourier_dft_magnitude(s2);
    dsp_buffer_reverse(s1->buf, s1->len);
    dsp_buffer_mul(s1, s2->buf, s1->len);
    dsp_buffer_copy(s1->buf, buf, s1->len);
    dsp_stream_free(s1);
    dsp_stream_free(s2);
>>>>>>> v1.7
    return buf;
}

double *VLBIBaseline::Correlate(double* buf, int idx)
{
    dsp_stream_p s1 = dsp_stream_copy(first);
<<<<<<< HEAD
    s1->buf = &s1->buf[idx];
    s1->len = second->len-idx;
    dsp_convolution_convolution(s1, second);
    dsp_buffer_copy(s1->buf, buf, (second->len-idx));
=======
    dsp_stream_set_buffer(s1, &s1->buf[idx], first->len-idx);
    dsp_buffer_reverse(s1->buf, s1->len);
    dsp_fourier_dft_magnitude(s1);
    dsp_fourier_dft_magnitude(second);
    dsp_stream_p s2 = dsp_convolution_convolution(s1, second);
    dsp_buffer_copy(s2->buf, buf, s2->len);
    dsp_stream_free(s2);
>>>>>>> v1.7
    dsp_stream_free(s1);
    return buf;
}

double VLBIBaseline::getUVSize()
{
    double res = vlbi_estimate_resolution_zero(LIGHTSPEED / Stream->lambda);
    baseline = vlbi_baseline_from_coordinates(first->location, second->location);
    res = vlbi_estimate_resolution(res, baseline[3]);
    return res;
}

double *VLBIBaseline::getUVCoords(double J2000_Offset_Time)
{
<<<<<<< HEAD
    double lst, ha, *az_coordinates_1, *az_coordinates_2;
    az_coordinates_1 = (double*)malloc(sizeof(double)*2);
    az_coordinates_2 = (double*)malloc(sizeof(double)*2);
=======
    double ha = J2000_Offset_Time - starttime;
    ha *= SIDEREAL_DAY / SOLAR_DAY;
    ha *= PI * 2 / SIDEREAL_DAY;
    ha += HA;
    while(ha < 0)
        ha += 2 * PI;
    while(ha >= 2 * PI)
        ha -= 2 * PI;
    double dec = Stream->target[1];
    dec *= PI / 180.0;
    return vlbi_calc_uv_coords(ha, dec, baseline_m, Stream->lambda);
}
>>>>>>> v1.7

    baseline = vlbi_baseline_from_coordinates(first->location, second->location);

    lst = vlbi_time_J2000time_to_lst(J2000_Offset_Time + starttime, first->location[1]);
    ha = vlbi_astro_get_local_hour_angle(lst, Stream->target[0]);
    vlbi_astro_get_alt_az_coordinates(ha, first->target[1], first->location[0], &az_coordinates_1[0], &az_coordinates_1[1]);

    lst = vlbi_time_J2000time_to_lst(J2000_Offset_Time + starttime, second->location[1]);
    ha = vlbi_astro_get_local_hour_angle(lst, Stream->target[0]);
    vlbi_astro_get_alt_az_coordinates(ha, second->target[1], second->location[0], &az_coordinates_2[0], &az_coordinates_2[1]);

    return vlbi_calc_baseline_projection(az_coordinates_1[0]-az_coordinates_2[0], az_coordinates_1[1]-az_coordinates_2[1], baseline[3]);
}

double *VLBIBaseline::getUVCoords(int index)
{
    double *baseline = vlbi_baseline_from_distance(&first->location[index * 3], &second->location[index * 3]);
    return vlbi_calc_baseline_projection(Stream->target[index * 2], Stream->target[index * 2+1], baseline[index * 4 + 3]);
}

void VLBIBaseline::setTarget(double *target)
{
    Stream->target = target;
}

void VLBIBaseline::setWaveLength(double wavelength)
{
    Stream->lambda = wavelength;
}

void VLBIBaseline::setSampleRate(double samplerate)
{
    Stream->samplerate = samplerate;
}

VLBIBaseline::~VLBIBaseline()
{
}
