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
#include "node.h"
#include "nodecollection.h"
#include "baselinecollection.h"
#include "modelcollection.h"

NodeCollection::NodeCollection() : VLBICollection::VLBICollection()
{
    relative = false;
    baselines = new BaselineCollection(this);
    models = new ModelCollection();
}

NodeCollection::~NodeCollection()
{
}

void NodeCollection::Add(VLBINode * element)
{
    VLBICollection::Add((intptr_t)element, element->getName());
    baselines->Update();
}

void NodeCollection::RemoveKey(const char* name)
{
    VLBICollection::RemoveKey(name);
    baselines->Update();
}

VLBINode * NodeCollection::Get(const char* name)
{
    return (VLBINode *)VLBICollection::Get(name);
}

void NodeCollection::Remove(VLBINode *element)
{
    VLBICollection::Remove((intptr_t)element);
    baselines->Update();
}

void NodeCollection::RemoveAt(int index)
{
    VLBICollection::RemoveAt(index);
    baselines->Update();
}

VLBINode * NodeCollection::At(int index)
{
    return (VLBINode *)(VLBICollection::At(index));
}

bool NodeCollection::Contains(VLBINode *element)
{
    return VLBICollection::Contains((intptr_t)element);
}

bool NodeCollection::ContainsKey(const char* element)
{
    return VLBICollection::ContainsKey(element);
}

int NodeCollection::IndexOf(VLBINode *element)
{
    return VLBICollection::IndexOf((intptr_t)element);
}

void NodeCollection::setRelative(bool value)
{
    relative = value;
    if(value)
    {
        for(int x = 0; x < Count; x++)
        {
            memcpy(At(x)->stationLocation().coordinates, station.coordinates, sizeof(dsp_location));
        }
        for(int x = 0; x < getBaselines()->Count; x++)
        {
            memcpy(getBaselines()->At(x)->stationLocation()->coordinates, station.coordinates, sizeof(dsp_location));
        }
    }
}
