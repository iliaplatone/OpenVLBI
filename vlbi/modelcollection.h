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

#ifndef _MODELCOLLECTION_H
#define _MODELCOLLECTION_H

#include "collection.h"

class ModelCollection : public VLBICollection
{
    public:
        ModelCollection();
        ~ModelCollection();
        void Add(dsp_stream_p element, const char *name = "");
        dsp_stream_p Get(const char* name);
        void Remove(const char* element);
        void RemoveAt(int index);
        dsp_stream_p  At(int index);
        bool Contains(const char* element);
        int IndexOf(dsp_stream_p element);
};

#endif //_MODELCOLLECTION_H
