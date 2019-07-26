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
#include "instancecollection.h"

InstanceCollection::InstanceCollection() : VLBICollection::VLBICollection()
{
}

InstanceCollection::~InstanceCollection()
{
}

void InstanceCollection::Add(vlbi_context element, const char *name)
{
    VLBICollection::Add(element, name);
}

void InstanceCollection::Remove(const char* name)
{
    VLBICollection::Remove(name);
}

vlbi_context InstanceCollection::Get(const char* name)
{
    return VLBICollection::Get(name);
}

void InstanceCollection::Remove(vlbi_context element)
{
    VLBICollection::Remove(element);
}

void InstanceCollection::RemoveAt(int index)
{
	VLBICollection::RemoveAt(index);
}

vlbi_context  InstanceCollection::At(int index)
{
	return (vlbi_context )(VLBICollection::At(index));
}

bool InstanceCollection::Contains(vlbi_context element)
{
    return VLBICollection::Contains(element);
}

bool InstanceCollection::ContainsKey(const char* element)
{
    return VLBICollection::ContainsKey(element);
}

int InstanceCollection::IndexOf(vlbi_context element)
{
    return VLBICollection::IndexOf(element);
}

