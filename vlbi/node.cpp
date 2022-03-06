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

#include <dsp.h>
#include "node.h"

VLBINode::VLBINode(dsp_stream_p stream, const char* name, int index, bool geographic_coordinates)
{
    Stream = stream;
    if(Stream->magnitude == nullptr) {
        Stream->magnitude = dsp_stream_copy(Stream);
        dsp_buffer_set(Stream->magnitude->buf, Stream->magnitude->len, 0.0);
    }
    if(Stream->phase == nullptr) {
        Stream->phase = dsp_stream_copy(Stream);
        dsp_buffer_set(Stream->phase->buf, Stream->phase->len, 0.0);
    }
    Name = (char*)calloc(150, 1);
    sprintf(Name, "%s", name);
    Index = index;
    setLocation(0);
    Geo = geographic_coordinates;
}

VLBINode::~VLBINode()
{
    free(getName());
}

void VLBINode::setSampleRate(double samplerate)
{
    getStream()->align_info.factor[0] = getStream()->samplerate / samplerate;
    if(getStream()->align_info.factor[0] != 1.0)
    {
        getStream()->sizes[0] *= getStream()->align_info.factor[0];
        getStream()->len *= getStream()->align_info.factor[0];
        dsp_stream_alloc_buffer(getStream(), getStream()->len);
        dsp_stream_scale(getStream());
    }
    getStream()->samplerate = samplerate;
}
