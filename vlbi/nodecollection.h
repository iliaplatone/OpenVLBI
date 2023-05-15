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
class ModelCollection;

class NodeCollection : public VLBICollection
{
    public:
        NodeCollection();
        ~NodeCollection();
        void Add(VLBINode *element);
        void RemoveAt(int index);
        VLBINode *Get(const char* name);
        void Remove(const char* element);
        VLBINode * At(int index);
        bool Contains(const char* element);
        int IndexOf(VLBINode *element);
        BaselineCollection* getBaselines();
        inline ModelCollection* getModels()
        {
            return models;
        }
        dsp_location *stationLocation()
        {
            return &station;
        }
        bool isRelative()
        {
            return relative;
        }
        void setRelative(bool value);
        inline void setCorrelationOrder(int order)
        {
            correlation_order = fmax(order, 2);
        }
        inline int getCorrelationOrder() { return correlation_order; }

    private:
        int  correlation_order {2};
        BaselineCollection *baselines;
        bool relative;
        dsp_location station;
        ModelCollection *models;
};

#endif //_NODECOLLECTION_H
