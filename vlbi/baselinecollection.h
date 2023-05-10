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

#include "collection.h"
#include "baseline.h"
#include "nodecollection.h"

class BaselineCollection : public VLBICollection
{
    public:
        BaselineCollection(NodeCollection *nodes, int order = 2);
        ~BaselineCollection();
        void Update();
        void Add(VLBIBaseline *element);
        void RemoveAt(int index);
        VLBIBaseline *Get(const char* name);
        void Remove(const char* element);
        void Clear();
        VLBIBaseline * At(int index);
        bool Contains(const char *element);
        int IndexOf(VLBIBaseline *element);
        void setTarget(double *target);
        void setRa(double ra);
        void setDec(double dec);
        void setDistance(double dist);
        inline double getRa()
        {
            return Ra;
        }
        inline double getDec()
        {
            return Dec;
        }
        void setFrequency(double frequency);
        void setSampleRate(double samplerate);
        void setDelegate(vlbi_func2_t delegate);
        void setRelative(bool rel);
        inline void setStream(dsp_stream_p stream)
        {
            Stream = stream;
        }
        inline dsp_stream_p getStream()
        {
            return Stream;
        }
        inline NodeCollection *getNodes()
        {
            return Nodes;
        }
        inline int getWidth()
        {
            return width;
        }
        inline int getHeight()
        {
            return height;
        }
        inline int getCorrelationOrder()
        {
            return correlation_order;
        }
        inline void setCorrelationOrder(int order)
        {
            correlation_order = order;
            Update();
        }
        inline void setWidth(int w)
        {
            width = w;
            int h = getHeight();
            dsp_stream_set_dim(getStream(), 0, w);
            dsp_stream_set_dim(getStream(), 1, h);
            dsp_stream_alloc_buffer(getStream(), getStream()->len);
            dsp_buffer_set(getStream()->buf, getStream()->len, 0);
        }
        inline void setHeight(int h)
        {
            height = h;
            int w = getWidth();
            dsp_stream_set_dim(getStream(), 0, w);
            dsp_stream_set_dim(getStream(), 1, h);
            dsp_stream_alloc_buffer(getStream(), getStream()->len);
            dsp_buffer_set(getStream()->buf, getStream()->len, 0);
        }
        inline bool isRelative()
        {
            return relative;
        }

    protected:
        int correlation_order;
        bool relative;
        double Ra, Dec, Dist;
        NodeCollection *Nodes;
        int width { 128 };
        int height { 128 };
        dsp_stream_p Stream;
};

#endif //_BASELINECOLLECTION_H
