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

#ifndef _BASELINECOLLECTION_H
#define _BASELINECOLLECTION_H

#include <collection.h>
#include <baseline.h>
#include <nodecollection.h>

class BaselineCollection : public VLBICollection
{
public:
    BaselineCollection(NodeCollection *nodes);
    ~BaselineCollection();
    void Update();
    void Add(VLBIBaseline *element);
    void Remove(VLBIBaseline *element);
    void RemoveAt(int index);
    VLBIBaseline *Get(const char* name);
    void RemoveKey(const char* element);
    VLBIBaseline * At(int index);
    bool Contains(VLBIBaseline *element);
    bool ContainsKey(const char *element);
    int IndexOf(VLBIBaseline *element);
    void SetTarget(double *target);
    void SetFrequency(double frequency);
    void SetSampleRate(double samplerate);
    void SetDelegate(vlbi_func2_t delegate);
    inline dsp_stream_p getStream() { return Stream; }
    inline NodeCollection *getNodes() { return Nodes; }
    inline double getWidth() { return getStream()->sizes[0]; }
    inline double getHeight() { return getStream()->sizes[1]; }
    inline double setWidth(double w) { getStream()->sizes[0] = w; dsp_stream_alloc_buffer(getStream(), getStream()->sizes[0] * getStream()->sizes[1]); }
    inline double setHeight(double h) { getStream()->sizes[1] = h; dsp_stream_alloc_buffer(getStream(), getStream()->sizes[0] * getStream()->sizes[1]); }

protected:
    NodeCollection *Nodes;
    double width;
    double height;
    dsp_stream_p Stream;
};

#endif //_BASELINECOLLECTION_H
