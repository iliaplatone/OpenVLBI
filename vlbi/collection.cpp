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
{
    S = sizeof(VLBIElement);
    Items = (VLBIElement*)malloc(S);
    count = 0;
}

VLBICollection::~VLBICollection()
{
    free(Items);
    Items = 0;
}

ssize_t VLBICollection::Count()
{
    return count;
}

void VLBICollection::Clear()
{
    Items = (VLBIElement*)realloc(Items, sizeof(VLBIElement));
    count = 0;
}

void VLBICollection::Add(void* el, const char* name)
{
    if(Contains(name))
        return;
    VLBIElement item;
    item.item = el;
    strcpy(item.name, name);
    count++;
    Items = (VLBIElement*)realloc(Items, S * Count());
    Items[Count() - 1] = item;
}

void VLBICollection::Remove(const char* name)
{
    if(!Items) return;
    if(!Contains(name))
        return;
    for(int i = 0; i < Count(); i++)
    {
        if(!strcmp(Items[i].name, name))
        {
            Items[i].item = 0;
            break;
        }
    }
    Defrag();
}

void* VLBICollection::Get(const char* name)
{
    if(!Items) return nullptr;
    for(int i = 0; i < Count(); i++)
    {
        if(!strcmp(Items[i].name, name))
        {
            return (void*)Items[i].item;
        }
    }
    return nullptr;
}

void* VLBICollection::At(ssize_t index)
{
    if(!Items) return nullptr;
    if(index < 0 || index >= Count())
    {
        return nullptr;
    }
    return (void*)Items[index].item;
}


bool VLBICollection::Contains(const char* name)
{
    if(!Items) return false;
    bool ret = false;
    for(int i = 0; i < Count(); i++)
    {
        if(!strcmp(name, Items[i].name))
        {
            ret = true;
            break;
        }
    }
    return ret;
}

void VLBICollection::Defrag()
{
    if(!Items) return;
    int n = Count();
    count = 0;
    for(int i = 0; i < n; i++)
    {
        if(Items[i].item != 0)
        {
            Items[Count()] = Items[i];
            count ++;
        }
    }
    Items = (VLBIElement*)realloc(Items, S * count);
}

