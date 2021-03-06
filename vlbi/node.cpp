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

#include <dsp.h>
#include "node.h"

VLBINode::VLBINode(dsp_stream_p stream, int index, bool geographic_coordinates)
{
    Stream = stream;
    Name = (char*)Stream->arg;
    Index = index;
    setLocation(0);
    Geo = geographic_coordinates;
}

VLBINode::~VLBINode()
{
    dsp_stream_free_buffer(Stream);
    dsp_stream_free(Stream);
}
