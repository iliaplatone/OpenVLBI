/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2023  Ilia Platone
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

void InstanceCollection::add(vlbi_context element, const char *name)
{
    VLBICollection::add(element, name);
}

void InstanceCollection::remove(const char* name)
{
    VLBICollection::remove(name);
}

vlbi_context InstanceCollection::get(const char* name)
{
    return (vlbi_context)VLBICollection::get(name);
}

vlbi_context  InstanceCollection::at(int index)
{
    return (vlbi_context)VLBICollection::at(index);
}

bool InstanceCollection::contains(const char* element)
{
    return VLBICollection::contains(element);
}


