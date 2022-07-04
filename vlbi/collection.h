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

#ifndef _COLLECTION_H
#define _COLLECTION_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include<map>
#include <vector>
#include <vlbi.h>

struct VLBIElement {
    void* item;
    char *name;
};

class VLBICollection
{
public:
	VLBICollection();
    ~VLBICollection();
    void Add(void* element, const char *name = "");
    void* Get(const char *name);
    void Remove(const char* name);
    void* At(ssize_t index);
    bool Contains(const char* name);
    ssize_t Count();
    void Clear();

private:
    VLBIElement *Items;
    ssize_t S;
    ssize_t count;
    void Defrag();
};

#endif //_COLLECTION_H
