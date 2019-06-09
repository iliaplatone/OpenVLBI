/*  OpenVLBI - Open Source Very Long Baseline Interferometry
    Copyright (C) 2017  Ilia Platone

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

#include "collection.h"


VLBICollection::VLBICollection()
{
    S = sizeof(long);
    collection = (long*)malloc(S);
	Count = 0;
}

VLBICollection::~VLBICollection()
{
	free(collection);
	collection = nullptr;
}

void VLBICollection::Add(void* el)
{
    long element = (long)el;
	Count++;
    collection = (long*)realloc(collection, S * Count);
    collection[Count - 1] = element;
}

void VLBICollection::Remove(void* el)
{
    long element = (long)el;
    if(!Contains(el))
		return;
	for(int i = 0; i < Count; i++)
	{
		if(element == collection[i])
		{
			collection[i] = 0;
		}
	}
	Defrag();
}

void VLBICollection::RemoveAt(int index)
{
	if(index >= Count)
		return;
	collection[index] = 0;
	Defrag();
}

void* VLBICollection::At(int index)
{
    if(index < 0 || index >= Count) {
        return NULL;
    }
    return (void*)collection[index];
}

bool VLBICollection::Contains(void* el)
{
    long element = (long)el;
	bool ret = false;
	for(int i = 0; i < Count; i++)
	{
		if(element == collection[i])
		{
			ret = true;
		}
	}
	return ret;
}

int VLBICollection::IndexOf(void* el)
{
    long element = (long)el;
	int ret = -1;
	for(int i = 0; i < Count; i++)
	{
		if(element == collection[i])
		{
			ret = i;
		}
	}
	return ret;
}

void VLBICollection::Defrag()
{
	int count = Count;
	Count = 0;
	for(int i = 0; i < count; i++)
	{
		if(collection[i] != 0)
		{
			collection[Count] = collection[i];
			Count ++;
		}
	}
    collection = (long*)realloc(collection, S * Count);
}

