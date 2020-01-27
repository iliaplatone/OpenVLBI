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
    deltatime = fabs(starttime1 - starttime2);
    if(!m)
        baseline = vlbi_calc_baseline_m(node1->location, node2->location);
    else
        baseline = vlbi_calc_baseline_m_xyz(node1->location, node2->location);
    baseline_center = vlbi_calc_baseline_center(node1->location, node2->location);
    baseline_rad = vlbi_calc_baseline_rad(node1->location, node2->location);
    dsp_buffer_stretch(first->buf, first->len, 0.0, 1.0);
    dsp_buffer_stretch(second->buf, second->len, 0.0, 1.0);
    Name = (char*)Stream->arg;
}

double VLBIBaseline::Correlate(double J2000_Offset_Time)
{
    double delay = vlbi_calc_baseline_delay(first->location, second->location, Stream->target, J2000_Offset_Time);
    int i = (int)(((J2000_Offset_Time - starttime) + deltatime + fabs(delay)) * Stream->samplerate);
    int l = second->len-(int)((J2000_Offset_Time - starttime) * Stream->samplerate)-1;
    if(i < 0 || i >= first->len || l < 1 || l >= first->len)
       return 0.0;
    double r = 0.0;
    for(int x = l; x >= 0; x--)
        r += first->buf[i] * second->buf[x];
    return r / l;
}

double VLBIBaseline::Correlate(int i)
{
    int l = first->len - i - 1;
    double r = 0.0;
    for(int x = l; x >= 0; x--)
        r += first->buf[i] * second->buf[x];
    return r / l;
}

double VLBIBaseline::getUVSize()
{
    double res = vlbi_estimate_resolution_zero(LIGHTSPEED / Stream->lambda);
    res = vlbi_estimate_resolution(res, baseline[3]);
    return res;
}

double *VLBIBaseline::getUVCoords(double J2000_Offset_Time)
{
    double lst = vlbi_time_J2000time_to_lst(J2000_Offset_Time, baseline_center[1]);
    double ha = vlbi_astro_get_local_hour_angle(lst, Stream->target[0]);
    double dec = Stream->target[1];
    return vlbi_calc_uv_coords(ha, dec, baseline, Stream->lambda);
}

double *VLBIBaseline::getUVCoords()
{
    return vlbi_get_uv_coords(baseline, Stream->lambda);
}

double *VLBIBaseline::getUVCoords(int index)
{
    return vlbi_get_uv_coords_vector(baseline, Stream->lambda, (double*)(&Stream->target[index * 3]));
}

double *VLBIBaseline::getBaselineRad()
{
    return baseline_rad;
}

double *VLBIBaseline::getBaselineM()
{
    return baseline;
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
