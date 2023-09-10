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

#include "baseline.h"

VLBIBaseline::VLBIBaseline(VLBINode **nodes, int num_nodes)
{
    dsp_stream_p s = dsp_stream_new();
    s->is_copy ++;
    setStream(s);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    nodes_count = num_nodes;
    if(nodes_count > 1) {
        Nodes = nodes;
        sprintf(Name, "%s", getNode(0)->getName());
        for(int i = 1; i < nodes_count; i++) {
            char* n = strcat(Name, "*");
            n = strcat(Name, getNode(i)->getName());
        }
    }
    setRelative(false);
}

VLBIBaseline::VLBIBaseline(VLBINode *node1, VLBINode *node2)
{
    dsp_stream_p s = dsp_stream_new();
    s->is_copy ++;
    setStream(s);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    nodes_count = 2;
    Nodes = (VLBINode**)malloc(sizeof(VLBINode*) * nodes_count);
    Nodes[0] = node1;
    Nodes[1] = node2;
    sprintf(Name, "%s*%s", getNode(0)->getName(), getNode(1)->getName());
    setRelative(false);
}

VLBIBaseline::~VLBIBaseline()
{
    if(Stream->is_copy > 0)
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

double VLBIBaseline::Correlate(double *times)
{
    int *indexes = (int*)malloc(sizeof(int)*nodes_count);
    for(int i = 0; i < nodes_count; i++)
        indexes[i] = (times[i] - getStartTime()) / getSampleRate();
    double val = Correlate(indexes);
    free(indexes);
    return val;
}

double VLBIBaseline::Correlate(int *indexes)
{
    double val = 0.0;
    int i = 0;
    if(indexes[i] >= 0 && indexes[i] < getNode(i)->getStream()->len) {
        val = getNode(i)->getStream()->buf[indexes[i]];
        for(i = 1; i < nodes_count; i++)
            if(indexes[i] >= 0 && indexes[i] < getNode(i)->getStream()->len)
                val = dsp_correlation_delegate(val, getNode(i)->getStream()->buf[indexes[i]]);
    }
    return val;
}

double VLBIBaseline::getStartTime()
{
    double starttime = DBL_MIN;
    for(int i = 0; i < nodes_count; i++)
        starttime = fmax(starttime, getNode(i)->getStartTime());
    return starttime;
}

double VLBIBaseline::getEndTime()
{
    double tau = 1.0 / getSampleRate();
    double endtime = DBL_MAX;
    for(int i = 0; i < nodes_count; i++)
        endtime = getStartTime() + fmin(endtime, getNode(i)->getStream()->len) * tau;
    return endtime;
}

double *VLBIBaseline::getBaseline()
{
    double *b;
    dsp_location baseline;
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
            baseline.geographic.lat = (lat - baseline.geographic.lat);
            baseline.geographic.lon = (lon - baseline.geographic.lon);
            baseline.geographic.el = (el - baseline.geographic.el);
        }
        while(baseline.geographic.lon >= 180)
            baseline.geographic.lon -= 360;
        while(baseline.geographic.lon < -180)
            baseline.geographic.lon += 360;
        b = vlbi_matrix_calc_location(baseline.coordinates);
    }
    else
    {
        b = (double*)malloc(sizeof(double)*3);
        baseline.xyz.x = getNode(0)->getLocation()[0];
        baseline.xyz.y = getNode(0)->getLocation()[1];
        baseline.xyz.z = getNode(0)->getLocation()[2];
        for(int i = 1; i < nodes_count; i++)
        {
            double x = getNode(i)->getLocation()[0];
            double y = getNode(i)->getLocation()[1];
            double z = getNode(i)->getLocation()[2];
            baseline.xyz.x = (x - baseline.xyz.x);
            baseline.xyz.y = (y - baseline.xyz.y);
            baseline.xyz.z = (z - baseline.xyz.z);
        }
        memcpy(b, baseline.coordinates, sizeof(double)*3);
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
        fmod(center.geographic.lon, 360.0);
        vlbi_astro_alt_az_from_ra_dec(time, Ra, Dec, center.geographic.lat, center.geographic.lon, &Alt, &Az);
    }
    else
    {
        vlbi_astro_alt_az_from_ra_dec(time, Ra, Dec, stationLocation()->geographic.lat, stationLocation()->geographic.lon, &Alt,
                                      &Az);
    }
    setTarget(Az, Alt, Dist);
}
