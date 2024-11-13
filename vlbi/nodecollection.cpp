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
    models = new ModelCollection();
    baselines = new BaselineCollection(this);
    setCorrelationOrder(2);
}

NodeCollection::~NodeCollection()
{
}

BaselineCollection* NodeCollection::getBaselines()
{
    for(int x = 0; x < baselines->count(); x++)
    {
        memcpy(baselines->at(x)->stationLocation()->coordinates, station.coordinates, sizeof(dsp_location));
    }
    return baselines;
}

void NodeCollection::add(VLBINode * element)
{
    VLBICollection::add(element, element->getName());
    setCorrelationOrder(getCorrelationOrder());
}

void NodeCollection::remove(const char* name)
{
    VLBICollection::remove(name);
    setCorrelationOrder(getCorrelationOrder());
}

VLBINode * NodeCollection::get(const char* name)
{
    return (VLBINode *)VLBICollection::get(name);
}

VLBINode * NodeCollection::at(int index)
{
    return (VLBINode *)(VLBICollection::at(index));
}

bool NodeCollection::contains(const char* element)
{
    return VLBICollection::contains(element);
}

void NodeCollection::setRelative(bool value)
{
    relative = value;
    baselines->setRelative(value);
    for(int x = 0; x < count(); x++)
    {
        memcpy(at(x)->stationLocation().coordinates, station.coordinates, sizeof(dsp_location));
    }
}

void NodeCollection::setCorrelationOrder(int order)
{
    correlation_order = fmax(order, fmin(count(), 2));
    if(count() < correlation_order) return;
    baselines->setCorrelationOrder(order);
}
