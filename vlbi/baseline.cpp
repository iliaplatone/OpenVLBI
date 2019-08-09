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
    timediff = 0;
    if(starttime1 < starttime2) {
        first = node1;
        second = node2;
    } else {
        first = node2;
        second = node1;
    }
    dsp_fourier_dft_magnitude(first);
    dsp_fourier_dft_magnitude(second);
    timediff = fabs(starttime1 - starttime2);
    if(!m)
        baseline_m = vlbi_calc_baseline_m(node1->location, node2->location);
    else
        baseline_m = vlbi_calc_baseline_m_xyz(node1->location, node2->location);
    baseline_center = vlbi_calc_baseline_center(node1->location, node2->location);
    baseline_rad = vlbi_calc_baseline_rad(node1->location, node2->location);
    Name = (char*)Stream->arg;
}

double *VLBIBaseline::Correlate(double* buf, double J2000_Offset_Time, int *idx)
{
    double delay = vlbi_calc_baseline_delay(first->location, second->location, Stream->target, J2000_Offset_Time);
    int i = (int)(((J2000_Offset_Time - starttime) + timediff + delay) * first->samplerate);
    for (int x=0; x < second->len-i; x++) {
        buf[x] = first->buf[x + i] * second->buf[second->len - x - 1];
    }
    *idx = i;
    return buf;
}

double *VLBIBaseline::Correlate(double* buf, int idx)
{
    for (int x=0; x < second->len; x++) {
        buf[x] = first->buf[x] * second->buf[second->len - x - 1];
    }
    return buf;
}

double VLBIBaseline::getUVSize()
{
    double res = vlbi_estimate_resolution_zero(LIGHTSPEED / Stream->lambda);
    res = vlbi_estimate_resolution(res, baseline_m[3]);
    return res;
}

double *VLBIBaseline::getUVCoords(double J2000_Offset_Time)
{
    double ha = J2000_Offset_Time - starttime;
    ha *= SIDEREAL_DAY / SOLAR_DAY;
    ha *= PI * 2 / SIDEREAL_DAY;
    ha += HA;
    while(ha < 0)
        ha += 2 * PI;
    while(ha >= 2 * PI)
        ha -= 2 * PI;
    if(ha < (PI / 2) || ha > (PI * 3 / 2))
        return NULL;
    double dec = Stream->target[1];
    dec *= PI / 180.0;
    return vlbi_calc_uv_coords(ha, dec, baseline_m, Stream->lambda);
}

double *VLBIBaseline::getUVCoords()
{
    return vlbi_get_uv_coords(baseline_m, Stream->lambda);
}

double *VLBIBaseline::getUVCoords(int index)
{
    return vlbi_get_uv_coords_vector(baseline_m, Stream->lambda, (double*)(&Stream->target[index * 3]));
}

double *VLBIBaseline::getBaselineRad()
{
    return baseline_rad;
}

double *VLBIBaseline::getBaselineM()
{
    return baseline_m;
}

double *VLBIBaseline::getBaselineCenter()
{
    return baseline_center;
}

void VLBIBaseline::setTarget(double *target)
{
    memcpy(Stream->target, target, sizeof(double) * 3);
    double lst = vlbi_time_J2000time_to_lst(starttime, second->location[1]);
    HA = vlbi_astro_get_local_hour_angle(lst, Stream->target[0]);
    HA *= PI / 180.0;
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
