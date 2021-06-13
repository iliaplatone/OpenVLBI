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

double VLBIBaseline::getStartTime()
{
    double starttime1 = getNode1()->getStartTime();
    double starttime2 = getNode2()->getStartTime();
    double starttime = fmax(starttime1, starttime2);
    return fmax(starttime, starttime+fabs(getProjection()[2]));
}

double *VLBIBaseline::getBaseline()
{
    double *bl = vlbi_calc_baseline(getNode1()->getLocation(), getNode2()->getLocation());
    baseline[0] = bl[0];
    baseline[1] = bl[1];
    baseline[2] = bl[2];
    free (bl);
    return baseline;
}

double *VLBIBaseline::getProjection()
{
    double *tmp = vlbi_calc_3d_projection(Target[1], Target[0], baseline);
    double *proj = vlbi_calc_uv_coordinates(tmp, getWavelength());
    free (tmp);
    projection[0] = proj[0];
    projection[1] = proj[1];
    projection[2] = proj[2];
    free (proj);
    return projection;
}
