/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2022  Ilia Platone
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <cstdio>
#include <cstdlib>
#include "nodecollection.h"
#include "baseline.h"
#include "baselinecollection.h"


BaselineCollection::BaselineCollection(NodeCollection *nodes, int order) : VLBICollection::VLBICollection()
{
    correlation_order = order;
    Nodes = nodes;
    Stream = dsp_stream_new();
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    setWidth(128);
    setHeight(128);
    dsp_buffer_set(getStream()->buf, getStream()->len, 0);
}

BaselineCollection::~BaselineCollection()
{
    for(int i = 0; i < Count(); i++)
    {
        At(i)->~VLBIBaseline();
    }
}

void BaselineCollection::Update()
{
    this->Clear();
    VLBINode** nodes = new VLBINode*[getCorrelationOrder()];
    for(int i = 0; i < getNodes()->Count() * (getNodes()->Count() - 1) / 2; i++)
    {
        for(int o = 0; o < getCorrelationOrder(); o++) {
            int idx = (i + o * ((1 + i) / getNodes()->Count())) % getNodes()->Count();
            nodes[o] = getNodes()->At(idx);
        }
        VLBIBaseline *b = new VLBIBaseline(nodes, getCorrelationOrder());
        b->getStream()->parent = Stream;
        this->Add(b);
    }
    setRelative(isRelative());
    setRa(getRa());
    setDec(getDec());
}

void BaselineCollection::Add(VLBIBaseline * element)
{
    element->getStream()->parent = Stream;
    VLBICollection::Add(element, element->getName());
}

void BaselineCollection::Remove(const char* name)
{
    VLBICollection::Remove(name);
}

void BaselineCollection::Clear()
{
}

VLBIBaseline * BaselineCollection::Get(const char* name)
{
    return (VLBIBaseline *)VLBICollection::Get(name);
}

VLBIBaseline * BaselineCollection::At(int index)
{
    return (VLBIBaseline *)(VLBICollection::At(index));
}

bool BaselineCollection::Contains(const char* element)
{
    return VLBICollection::Contains(element);
}

void BaselineCollection::SetTarget(double *target)
{
    memcpy(Stream->target, target, sizeof(double) * 3);
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setTarget(Stream->target);
    }
}

void BaselineCollection:: setRa(double ra)
{
    Ra = ra;
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setRa(ra);
    }
}

void BaselineCollection:: setDec(double dec)
{
    Dec = dec;
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setDec(dec);
    }
}

void BaselineCollection:: setDistance(double dist)
{
    Dist = dist;
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setDistance(dist);
    }
}

void BaselineCollection::SetFrequency(double frequency)
{
    Stream->wavelength = vlbi_astro_mean_speed(0) / frequency;
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setWaveLength(vlbi_astro_mean_speed(0) / frequency);
    }
}

void BaselineCollection::setRelative(bool rel)
{
    relative = rel;
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setRelative(rel);
    }
}

void BaselineCollection::SetSampleRate(double samplerate)
{
    Stream->samplerate = samplerate;
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setSampleRate(samplerate);
    }
}

void BaselineCollection::SetDelegate(vlbi_func2_t delegate)
{
    for(int i = 0; i < Count(); i++)
    {
        At(i)->setDelegate(delegate);
    }
}
