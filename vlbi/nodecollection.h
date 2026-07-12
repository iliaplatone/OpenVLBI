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
        void add(VLBINode *element);
        void removeAt(int index);
        VLBINode *get(const char* name);
        void remove(const char* element);
        VLBINode * at(int index);
        bool contains(const char* element);
        int indexOf(VLBINode *element);
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
        void setCorrelationOrder(int order);
        inline int getCorrelationOrder() { return correlation_order; }

    private:
        int  correlation_order {2};
        BaselineCollection *baselines;
        bool relative;
        dsp_location station;
        ModelCollection *models;
};

#endif //_NODECOLLECTION_H
