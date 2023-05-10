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

VLBIBaseline::VLBIBaseline(VLBINode **nodes, int num_nodes)
{
    setStream(dsp_stream_new());
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    Name = (char*)malloc(150);
    Nodes = nodes;
    nodes_count = num_nodes;
    if(nodes_count > 0) {
        sprintf(Name, "%s", getNode(0)->getName());
        for(int i = 1; i < nodes_count; i++)
            sprintf(Name, "%s_%s", Name, getNode(i)->getName());
    }
    setRelative(false);
}

VLBIBaseline::VLBIBaseline(VLBINode *node1, VLBINode *node2)
{
    setStream(dsp_stream_new());
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    nodes_count = 2;
    Name = (char*)malloc(150);
    Nodes = new VLBINode*[2] {node1, node2};
    if(nodes_count > 0) {
        sprintf(Name, "%s", getNode(0)->getName());
        for(int i = 1; i < nodes_count; i++)
            sprintf(Name, "%s_%s", Name, getNode(i)->getName());
    }
    setRelative(false);
}

VLBIBaseline::~VLBIBaseline()
{
    if(Stream->is_copy == 0)
        freeStream();
}

double VLBIBaseline::Correlate(double time)
{
    if(!Locked())
        return 0.0;
    int idx = (time - getStartTime()) / getSampleRate();
    if(idx >= 0 && idx < getStream()->len)
        return dsp_correlation_delegate(getStream()->dft.pairs[idx][0], getStream()->dft.pairs[idx][1]);
    return 0.0;
}

double VLBIBaseline::Correlate(double time1, double time2)
{
    int idx1 = (time1 - getStartTime()) / getSampleRate();
    int idx2 = (time2 - getStartTime()) / getSampleRate();
    if(idx1 >= 0 && idx2 >= 0 && idx1 < getNode(0)->getStream()->len && idx2 < getNode(1)->getStream()->len)
        return dsp_correlation_delegate(getNode(0)->getStream()->buf[idx1], getNode(1)->getStream()->buf[idx2]);
    return 0.0;
}

double VLBIBaseline::Correlate(int idx1, int idx2)
{
    if(idx1 > 0 && idx2 > 0 && idx1 < getNode(0)->getStream()->len && idx2 < getNode(1)->getStream()->len)
        return dsp_correlation_delegate(getNode(0)->getStream()->buf[idx1], getNode(1)->getStream()->buf[idx2]);
    return 0.0;
}

double VLBIBaseline::Correlate(int *indexes)
{
    double val = 0.0;
    int i = 0;
    if(indexes[i] > 0 && indexes[i] < getNode(i)->getStream()->len)
        val = getNode(i)->getStream()->buf[indexes[i]];
    for(int i = 1; i < nodes_count; i++)
        if(indexes[i] > 0 && indexes[i] < getNode(i)->getStream()->len)
            val = dsp_correlation_delegate(val, getNode(i)->getStream()->buf[indexes[i]]);
    return val;
}

double VLBIBaseline::getStartTime()
{
    double starttime = 0.0;
    for(int i = 1; i < nodes_count; i++)
        starttime = fmax(starttime, getNode(i)->getStartTime());
    return starttime;
}

double VLBIBaseline::getEndTime()
{
    double tau = 1.0 / getSampleRate();
    double endtime = DBL_MAX;
    for(int i = 1; i < nodes_count; i++)
        endtime = getStartTime() + fmin(endtime, getNode(i)->getStream()->len) * tau;
    return endtime;
}

double *VLBIBaseline::getBaseline()
{
    double *b;
    dsp_location location1, location2, baseline;
    if (!isRelative())
    {
        baseline.geographic.lat = getNode(0)->getGeographicLocation()[0];
        baseline.geographic.lon = getNode(0)->getGeographicLocation()[1];
        baseline.geographic.el = getNode(0)->getGeographicLocation()[2];
        for(int i = 1; i < nodes_count; i++)
        {
            double lat = getNode(i)->getGeographicLocation()[0];
            double lon = getNode(i)->getGeographicLocation()[1];
            double el = getNode(i)->getGeographicLocation()[2];
            baseline.geographic.lon = (lon - baseline.geographic.lon);
            baseline.geographic.el = (el - baseline.geographic.el);
        }
        while(baseline.geographic.lon >= 180)
            baseline.geographic.lon -= 360;
        while(baseline.geographic.lon < -180)
            baseline.geographic.lon += 360;
        while(baseline.geographic.lat >= 90.0)
            baseline.geographic.lat = 180.0 - baseline.geographic.lat;
        while(baseline.geographic.lat < -90.0)
            baseline.geographic.lat = -180.0 + baseline.geographic.lat;
        b = vlbi_matrix_calc_location(baseline.coordinates);
        memcpy(baseline.coordinates, b, sizeof(dsp_location));
        baseline.xyz.z -= getNode(1)->getGeographicLocation()[2];
    }
    else
    {
        memcpy(location1.coordinates, getNode(0)->getLocation(), sizeof(double) * 3);
        memcpy(location2.coordinates, getNode(1)->getLocation(), sizeof(double) * 3);
        b = (double*)malloc(sizeof(double) * 3);
        b[0] = location1.xyz.x - location2.xyz.x;
        b[1] = location1.xyz.y - location2.xyz.y;
        b[2] = location1.xyz.z - location2.xyz.z;
    }
    return b;
}

void VLBIBaseline::getProjection()
{
    double *b = getBaseline();
    double *tmp = vlbi_matrix_calc_parametric_projection(Target, b);
    free (b);
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
        center.geographic.lat = 0.0;
        center.geographic.lon = 0.0;
        center.geographic.el = 0.0;
        for(int i = 0; i < nodes_count; i ++) {
            center.geographic.lat += getNode(i)->getGeographicLocation()[0];
            center.geographic.el += getNode(i)->getGeographicLocation()[2];
            double lon = getNode(i)->getGeographicLocation()[1];
            if(fabs(center.geographic.lon - lon) >= 180.0)
            {
                if(lon < 180.0)
                    lon += 360;
            }
            center.geographic.lon += lon;
        }
        center.geographic.lat /= nodes_count;
        center.geographic.lon /= nodes_count;
        center.geographic.el /= nodes_count;
        if(center.geographic.lon >= 360)
            center.geographic.lon -= 360;
        if(center.geographic.lon < 0)
            center.geographic.lon += 360;
        vlbi_astro_alt_az_from_ra_dec(time, Ra, Dec, center.geographic.lat, center.geographic.lon, &Alt, &Az);
    }
    else
    {
        vlbi_astro_alt_az_from_ra_dec(time, Ra, Dec, stationLocation()->geographic.lat, stationLocation()->geographic.lon, &Alt,
                                      &Az);
    }
    setTarget(Az, Alt, Dist);
}
