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
    Stream->arg = calloc(150, 1);
    sprintf((char*)Stream->arg, "%s_%s", node1->getName(), node2->getName());
    Name = (char*)Stream->arg;
    Node1 = node1;
    Node2 = node2;
}

VLBIBaseline::~VLBIBaseline()
{
}

double VLBIBaseline::Correlate(double time, double offset)
{
    int idx = (time-getStartTime())/getSamplerate();
    int ofs = (offset+time-getStartTime())/getSamplerate();
    if(idx > 0 && ofs > 0 && idx < getNode1()->getStream()->len && ofs < getNode2()->getStream()->len)
        return dsp_correlation_delegate(getNode1()->getStream()->buf[idx], getNode2()->getStream()->buf[ofs]);
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
    return fmax(starttime, starttime+fabs(getProjection()[2]));
}

double *VLBIBaseline::getBaseline()
{
    dsp_location location1;
    dsp_location location2;
    if (getNode1()->GeographicCoordinates()) {
        memcpy(location1.coordinates, getNode1()->getGeographicLocation(), sizeof(double)*3);
        vlbi_calc_location(location1.coordinates);
    } else {
        memcpy(location1.coordinates, getNode1()->getLocation(), sizeof(double)*3);
    }

    if (getNode2()->GeographicCoordinates()) {
        memcpy(location2.coordinates, getNode2()->getGeographicLocation(), sizeof(double)*3);
        vlbi_calc_location(location2.coordinates);
    } else {
        memcpy(location2.coordinates, getNode2()->getLocation(), sizeof(double)*3);
    }
    double *bl = vlbi_calc_baseline(location1.coordinates, location2.coordinates);
    baseline[0] = bl[0];
    baseline[1] = bl[1];
    baseline[2] = bl[2];
    free (bl);
    return baseline;
}

double *VLBIBaseline::getProjection()
{
    double *tmp = vlbi_calc_3d_projection(Target[1], Target[0], getBaseline());
    double *proj = vlbi_calc_uv_coordinates(tmp, getWavelength());
    free (tmp);
    projection[0] = proj[0];
    projection[1] = proj[1];
    projection[2] = proj[2];
    free (proj);
    return projection;
}
