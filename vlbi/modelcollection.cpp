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

void ModelCollection::Add(dsp_stream_p element, const char *name)
{
    strcpy(element->name, name);
    VLBICollection::Add((intptr_t)element, name);
}

void ModelCollection::RemoveKey(const char* name)
{
    VLBICollection::RemoveKey(name);
}

dsp_stream_p ModelCollection::Get(const char* name)
{
    return (dsp_stream_p)VLBICollection::Get(name);
}

void ModelCollection::Remove(dsp_stream_p element)
{
    VLBICollection::Remove((intptr_t)element);
}

void ModelCollection::RemoveAt(int index)
{
    VLBICollection::RemoveAt(index);
}

dsp_stream_p  ModelCollection::At(int index)
{
    return (dsp_stream_p )(VLBICollection::At(index));
}

bool ModelCollection::Contains(dsp_stream_p element)
{
    return VLBICollection::Contains((intptr_t)element);
}

bool ModelCollection::ContainsKey(const char* element)
{
    return VLBICollection::ContainsKey(element);
}

int ModelCollection::IndexOf(dsp_stream_p element)
{
    return VLBICollection::IndexOf((intptr_t)element);
}

