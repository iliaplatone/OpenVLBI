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

using namespace std;

class VLBICollection : map <const char*, void*>
{
public:
	VLBICollection();
    ~VLBICollection();
    virtual void Add(void* element, const char *name = "");
    virtual void* Get(const char *name);
    virtual void Remove(const char* name);
    virtual void* At(size_t index);
    virtual bool Contains(const char* name);
    virtual ssize_t Count();
    virtual void Clear();
};

#endif //_COLLECTION_H
