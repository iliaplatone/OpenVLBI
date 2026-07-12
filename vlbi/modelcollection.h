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

#ifndef _MODELCOLLECTION_H
#define _MODELCOLLECTION_H

#include "collection.h"

class ModelCollection : public VLBICollection
{
    public:
        ModelCollection();
        ~ModelCollection();
        void add(dsp_stream_p element, const char *name = "");
        dsp_stream_p get(const char* name);
        void remove(const char* element);
        void removeAt(int index);
        dsp_stream_p  at(int index);
        bool contains(const char* element);
        int indexOf(dsp_stream_p element);
};

#endif //_MODELCOLLECTION_H
