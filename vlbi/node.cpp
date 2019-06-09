/*  OpenVLBI - Open Source Very Long Baseline Interferometry
    Copyright (C) 2017  Ilia Platone

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

#include <dsp.h>
#include "node.h"

VLBINode::VLBINode(dsp_stream_p stream)
{
    Stream = stream;
    Name = (char*)Stream->arg;
}

VLBINode::~VLBINode()
{
}

void VLBINode::setCoordinates(double lat, double lon, double el, bool osl)
{
    Stream->location[0] = lat;
    Stream->location[1] = lon;
    Stream->location[2] = el;
    if (osl) {
        Stream->location[2] = vlbi_calc_elevation_coarse(el, lat);
    }
    Stream->location = vlbi_calc_location_m(Stream->location);
}

void VLBINode::setCoordinates(double x, double y, double z)
{
    Stream->location[0] = x;
    Stream->location[1] = y;
    Stream->location[2] = z;
}
