/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2022  Ilia Platone
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

#include "baseline.h"

VLBIBaseline::VLBIBaseline(VLBINode *node1, VLBINode *node2)
{
    Stream = dsp_stream_new();
    Stream->magnitude = dsp_stream_new();
    Stream->phase = dsp_stream_new();
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    Name = (char*)calloc(150, 1);
    sprintf(Name, "%s_%s", node1->getName(), node2->getName());
    Node1 = node1;
    Node2 = node2;
    setRelative(false);
}

VLBIBaseline::~VLBIBaseline()
{
    dsp_stream_free_buffer(getStream());
    dsp_stream_free(getStream());
    free(getName());
}

double VLBIBaseline::Correlate(double time)
{
    if(!Locked())
        return 0.0;
    int idx = (time - getStartTime()) / getSampleRate();
    if(idx >= 0 && idx < getStream()->len)
        return dsp_correlation_delegate(getStream()->dft.fftw[idx][0], getStream()->dft.fftw[idx][1]);
    return 0.0;
}

double VLBIBaseline::Correlate(double time1, double time2)
{
    int idx1 = (time1 - getStartTime()) / getSampleRate();
    int idx2 = (time2 - getStartTime()) / getSampleRate();
    if(idx1 >= 0 && idx2 >= 0 && idx1 < getNode1()->getStream()->len && idx2 < getNode2()->getStream()->len)
        return dsp_correlation_delegate(getNode1()->getStream()->buf[idx1], getNode2()->getStream()->buf[idx2]);
    return 0.0;
}

double VLBIBaseline::Correlate(int idx1, int idx2)
{
    if(idx1 > 0 && idx2 > 0 && idx1 < getNode1()->getStream()->len && idx2 < getNode2()->getStream()->len)
        return dsp_correlation_delegate(getNode1()->getStream()->buf[idx1], getNode2()->getStream()->buf[idx2]);
    return 0.0;
}

double VLBIBaseline::getStartTime()
{
    double starttime1 = getNode1()->getStartTime();
    double starttime2 = getNode2()->getStartTime();
    double starttime = fmax(starttime1, starttime2);
    return starttime;
}

double VLBIBaseline::getEndTime()
{
    double tao = 1.0 / getSampleRate();
    double endtime = getStartTime() + fmin(getNode1()->getStream()->len, getNode2()->getStream()->len) * tao;
    return endtime;
}

double *VLBIBaseline::getBaseline()
{
    double *b;
    dsp_location location1, location2, baseline;
    if (!isRelative())
    {
        double lon1 = getNode1()->getGeographicLocation()[1];
        double lon2 = getNode2()->getGeographicLocation()[1];
        if(fabs(lon1 - lon2) >= 180.0)
        {
            if(lon1 < 180.0)
                lon1 += 360;
            if(lon2 < 180.0)
                lon2 += 360;
        }
        baseline.geographic.lon = (lon2 - lon1);
        if(baseline.geographic.lon >= 180)
            baseline.geographic.lon -= 360;
        if(baseline.geographic.lon < -180)
            baseline.geographic.lon += 360;
        baseline.geographic.lat = getNode2()->getGeographicLocation()[0];
        baseline.geographic.lat -= getNode1()->getGeographicLocation()[0];
        baseline.geographic.el = getNode2()->getGeographicLocation()[2];
        baseline.geographic.el -= getNode1()->getGeographicLocation()[2];
        baseline.geographic.el += getNode2()->getGeographicLocation()[2];
        b = vlbi_matrix_calc_location(baseline.coordinates);
        memcpy(baseline.coordinates, b, sizeof(dsp_location));
        baseline.xyz.z -= getNode2()->getGeographicLocation()[2];
    }
    else
    {
        memcpy(location1.coordinates, getNode1()->getLocation(), sizeof(double) * 3);
        memcpy(location2.coordinates, getNode2()->getLocation(), sizeof(double) * 3);
        b = (double*)malloc(sizeof(double) * 3);
        b[0] = location1.xyz.x - location2.xyz.x;
        b[1] = location1.xyz.y - location2.xyz.y;
        b[2] = location1.xyz.z - location2.xyz.z;
    }
    return b;
}

void VLBIBaseline::getProjection()
{
    double *tmp = vlbi_matrix_calc_3d_projection(Target[1], Target[0], getBaseline());
    double *proj = vlbi_matrix_calc_uv_coordinates(tmp, getWaveLength());
    free (tmp);
    u = proj[0];
    v = proj[1];
    delay = proj[2];
    free (proj);
}

void VLBIBaseline::setTime(double time)
{
    double Alt, Az;
    if(!isRelative())
    {
        dsp_location center;
        double lon1 = getNode1()->getGeographicLocation()[1];
        double lon2 = getNode2()->getGeographicLocation()[1];
        if(fabs(lon1 - lon2) >= 180.0)
        {
            if(lon1 < 180.0)
                lon1 += 360;
            if(lon2 < 180.0)
                lon2 += 360;
        }
        center.geographic.lon = (lon1 + lon2) / 2;
        if(center.geographic.lon >= 360)
            center.geographic.lon -= 360;
        if(center.geographic.lon < 0)
            center.geographic.lon += 360;
        center.geographic.lat = getNode1()->getGeographicLocation()[0];
        center.geographic.lat += getNode2()->getGeographicLocation()[0];
        center.geographic.lat /= 2;
        center.geographic.el = getNode1()->getGeographicLocation()[2];
        center.geographic.el += getNode2()->getGeographicLocation()[2];
        center.geographic.el /= 2;
        vlbi_astro_alt_az_from_ra_dec(time, Ra, Dec, center.geographic.lat, center.geographic.lon, &Alt, &Az);
    }
    else
    {
        vlbi_astro_alt_az_from_ra_dec(time, Ra, Dec, stationLocation()->geographic.lat, stationLocation()->geographic.lon, &Alt,
                                      &Az);
    }
    setTarget(Az, Alt);
}
