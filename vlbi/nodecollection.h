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

#ifndef _NODECOLLECTION_H
#define _NODECOLLECTION_H

#include "collection.h"
#include "node.h"

class BaselineCollection;

class NodeCollection : public VLBICollection
{
public:
	NodeCollection();
	~NodeCollection();
	void Add(VLBINode *element, const char* name = "");
	void Remove(VLBINode *element);
	void RemoveAt(int index);
        VLBINode *Get(const char* name);
        void RemoveKey(const char* element);
	VLBINode * At(int index);
	bool Contains(VLBINode *element);
	bool ContainsKey(const char* element);
    int IndexOf(VLBINode *element);
    inline BaselineCollection* getBaselines() { return baselines; }

private:
    BaselineCollection *baselines;
};

#endif //_NODECOLLECTION_H
