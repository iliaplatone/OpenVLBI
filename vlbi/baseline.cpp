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

VLBIBaseline::VLBIBaseline(VLBINode *node1, VLBINode *node2)
{
    Stream = dsp_stream_new();
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    Stream->arg = calloc(150, 1);
    sprintf(Name, "%s_%s", node1->getName(), node2->getName());
    Name = (char*)Stream->arg;
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
    int idx = (time-getStartTime())/getSampleRate();
    if(idx > 0 && idx < getStream()->len)
        return getStream()->buf[idx];
    return 0.0;
}

double VLBIBaseline::Correlate(double time1, double time2)
{
    int idx1 = (time1-getStartTime())/getSampleRate();
    int idx2 = (time2-getStartTime())/getSampleRate();
    if(idx1 > 0 && idx2 > 0 && idx1 < getNode1()->getStream()->len && idx2 < getNode2()->getStream()->len)
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
    dsp_location location1;
    dsp_location location2;
    if (!isRelative()) {
        double *coords = vlbi_calc_location(getNode1()->getGeographicLocation());
        memcpy(location1.coordinates, coords, sizeof(double)*3);
        free(coords);
        coords = vlbi_calc_location(getNode2()->getGeographicLocation());
        memcpy(location1.coordinates, coords, sizeof(double)*3);
        free(coords);
    } else {
        memcpy(location1.coordinates, getNode1()->getLocation(), sizeof(double)*3);
        memcpy(location2.coordinates, getNode2()->getLocation(), sizeof(double)*3);
    }

    double *bl = vlbi_calc_baseline(location1.coordinates, location2.coordinates);
    baseline[0] = bl[0];
    baseline[1] = bl[1];
    baseline[2] = bl[2];
    free (bl);
    return baseline;
}

void VLBIBaseline::getProjection()
{
    double *tmp = vlbi_calc_3d_projection(Target[1], Target[0], getBaseline());
    double *proj = vlbi_calc_uv_coordinates(tmp, getWaveLength());
    free (tmp);
    u = proj[0];
    v = proj[1];
    delay = proj[2];
    free (proj);
}
