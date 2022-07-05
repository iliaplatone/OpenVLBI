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

#include "collection.h"

VLBICollection::VLBICollection()
    : map<const char*, void*>()
{
}

VLBICollection::~VLBICollection()
{
}

void VLBICollection::Add(void* el, const char* name)
{
    if(!Contains(name)) {
        (*this)[name] = el;
    }
}

void VLBICollection::Remove(const char* name)
{
    if(!Contains(name))
        return;
    iterator i = this->begin();
    while (strcmp((*i).first, name))
        i ++;
    this->erase(i);
}

void* VLBICollection::Get(const char* name)
{
    for(iterator i = this->begin(); i != this->end(); i++)
    {
        if(!strcmp((*i).first, name))
        {
            return (void*)(*i).second;
        }
    }
    return nullptr;
}

void* VLBICollection::At(size_t index)
{
    if(index < 0 || index >= this->size())
    {
        return nullptr;
    }
    int x = 0;
    iterator i = this->begin();
    while (x++ < index)
        i ++;
    return (void*)((*i).second);
}

ssize_t VLBICollection::Count()
{
    return (ssize_t)(this->size());
}

void VLBICollection::Clear()
{
    this->clear();
}

bool VLBICollection::Contains(const char* name)
{
    return (this->count(name) > 0);
}

