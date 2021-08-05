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

VLBINode::VLBINode(dsp_stream_p stream, char* name, int index, bool geographic_coordinates)
{
    Stream = stream;
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    Name = name;
    Index = index;
    setLocation(0);
    Geo = geographic_coordinates;
}

VLBINode::~VLBINode()
{
    dsp_stream_free_buffer(getStream());
    dsp_stream_free(getStream());
    free(getName());
}
