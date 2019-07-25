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
#ifndef _INSTANCECOLLECTION_H
#define _INSTANCECOLLECTION_H

#include "collection.h"

class InstanceCollection : public VLBICollection
{
public:
	InstanceCollection();
	~InstanceCollection();
	void Add(vlbi_context element, const char *name = "");
	vlbi_context Get(const char* name);
	void Remove(const char* element);
	void Remove(vlbi_context name);
	void RemoveAt(int index);
	vlbi_context  At(int index);
	bool Contains(vlbi_context element);
    int IndexOf(vlbi_context element);
};

#endif //_INSTANCECOLLECTION_H
