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

#include <cstdio>
#include <cstdlib>
#include "modelcollection.h"

ModelCollection::ModelCollection() : VLBICollection::VLBICollection()
{
}

ModelCollection::~ModelCollection()
{
}

void ModelCollection::add(dsp_stream_p element, const char *name)
{
    strcpy(element->name, name);
    VLBICollection::add(element, name);
}

void ModelCollection::remove(const char* name)
{
    VLBICollection::remove(name);
}

dsp_stream_p ModelCollection::get(const char* name)
{
    return (dsp_stream_p)VLBICollection::get(name);
}

dsp_stream_p  ModelCollection::at(int index)
{
    return (dsp_stream_p )(VLBICollection::at(index));
}

bool ModelCollection::contains(const char* element)
{
    return VLBICollection::contains(element);
}

