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

#include <vlbi.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

struct VLBIElement {
	long item;
	char name[32];
};

class VLBICollection
{
public:
	VLBICollection();
	~VLBICollection();
	void Add(void* element, const char *name = "");
	void* Get(const char *name);
	void RemoveKey(const char* name);
	void Remove(void* element);
	void RemoveAt(int index);
	void* At(int index);
	bool Contains(void* element);
	bool ContainsKey(const char* name);
	int IndexOf(void* element);

	int Count;

private:
	void Defrag();
	VLBIElement* Items;
	ssize_t S;
};

#endif //_COLLECTION_H
