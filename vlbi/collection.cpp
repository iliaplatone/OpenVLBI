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

#include "collection.h"

VLBICollection::VLBICollection()
{
    S = sizeof(VLBIElement);
    Items = (VLBIElement*)malloc(S);
    Count = 0;
}

VLBICollection::~VLBICollection()
{
    free(Items);
    Items = 0;
}

ssize_t VLBICollection::count()
{
    return Count;
}

void VLBICollection::clear()
{
    Items = (VLBIElement*)realloc(Items, sizeof(VLBIElement));
    Count = 0;
}

void VLBICollection::add(void* el, const char* name)
{
    if(contains(name))
        return;
    VLBIElement item;
    item.item = el;
    strcpy(item.name, name);
    Count++;
    Items = (VLBIElement*)realloc(Items, S * count());
    Items[count() - 1] = item;
}

void VLBICollection::remove(const char* name)
{
    if(!Items) return;
    if(!contains(name))
        return;
    for(int i = 0; i < count(); i++)
    {
        if(!strcmp(Items[i].name, name))
        {
            Items[i].item = 0;
            break;
        }
    }
    defrag();
}

void* VLBICollection::get(const char* name)
{
    if(!Items) return nullptr;
    for(int i = 0; i < count(); i++)
    {
        if(!strcmp(Items[i].name, name))
        {
            return (void*)Items[i].item;
        }
    }
    return nullptr;
}

void* VLBICollection::at(ssize_t index)
{
    if(!Items) return nullptr;
    if(index < 0 || index >= count())
    {
        return nullptr;
    }
    return (void*)Items[index].item;
}


bool VLBICollection::contains(const char* name)
{
    if(!Items) return false;
    bool ret = false;
    for(int i = 0; i < count(); i++)
    {
        if(!strcmp(name, Items[i].name))
        {
            ret = true;
            break;
        }
    }
    return ret;
}

void VLBICollection::defrag()
{
    if(!Items) return;
    int n = count();
    Count = 0;
    for(int i = 0; i < n; i++)
    {
        if(Items[i].item != 0)
        {
            Items[count()] = Items[i];
            Count ++;
        }
    }
    Items = (VLBIElement*)realloc(Items, S * Count);
}

