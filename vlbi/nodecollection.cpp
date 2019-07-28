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

#include <cstdio>
#include <cstdlib>
#include "node.h"
#include "nodecollection.h"

NodeCollection::NodeCollection() : VLBICollection::VLBICollection()
{
}

NodeCollection::~NodeCollection()
{
}

void NodeCollection::Add(VLBINode * element, const char *name)
{
    VLBICollection::Add(element, name);
}

void NodeCollection::Remove(const char* name)
{
    VLBICollection::Remove(name);
}

VLBINode * NodeCollection::Get(const char* name)
{
    return (VLBINode *)VLBICollection::Get(name);
}

void NodeCollection::Remove(VLBINode *element)
{
    VLBICollection::Remove(element);
}

void NodeCollection::RemoveAt(int index)
{
	VLBICollection::RemoveAt(index);
}

VLBINode * NodeCollection::At(int index)
{
	return (VLBINode *)(VLBICollection::At(index));
}

bool NodeCollection::Contains(VLBINode *element)
{
    return VLBICollection::Contains(element);
}

bool NodeCollection::ContainsKey(const char* element)
{
    return VLBICollection::ContainsKey(element);
}

int NodeCollection::IndexOf(VLBINode *element)
{
    return VLBICollection::IndexOf(element);
}

