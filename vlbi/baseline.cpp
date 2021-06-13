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
}

VLBIBaseline::~VLBIBaseline()
{
}

double VLBIBaseline::Correlate(double time)
{
    int idx = (time-getStartTime())/getSamplerate();
    int offset = (int)(idx+(getProjection()[2]/getSamplerate()));
    return *dsp_correlation_delegate(getNode1()->getStream()->buf[idx+(int)fmax(0, offset)], getNode2()->getStream()->buf[idx+(int)fmax(0, -offset)]);
}

double VLBIBaseline::Correlate(int idx1, int idx2)
{
    return *dsp_correlation_delegate(getNode1()->getStream()->buf[idx1], getNode2()->getStream()->buf[idx2]);
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
        location1.geographic.lat = getNode1()->getLocation()[0];
        location1.geographic.lon = getNode1()->getLocation()[1];
        location1.geographic.el = vlbi_astro_estimate_geocentric_elevation(getNode1()->getLocation()[0], getNode1()->getLocation()[2]);
        vlbi_calc_location(location1.coordinates);
    }
    if (getNode2()->GeographicCoordinates()) {
        location2.geographic.lat = getNode2()->getLocation()[0];
        location2.geographic.lon = getNode2()->getLocation()[1];
        location2.geographic.el = vlbi_astro_estimate_geocentric_elevation(getNode2()->getLocation()[0], getNode2()->getLocation()[2]);
        vlbi_calc_location(location2.coordinates);
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
