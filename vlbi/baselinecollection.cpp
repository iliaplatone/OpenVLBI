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

#include <cstdio>
#include <cstdlib>
#include "nodecollection.h"
#include "baseline.h"
#include "baselinecollection.h"


BaselineCollection::BaselineCollection(NodeCollection *nodes, int order) : VLBICollection::VLBICollection()
{
    Nodes = nodes;
    Stream = dsp_stream_new();
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_add_dim(getStream(), 1);
    dsp_stream_alloc_buffer(getStream(), getStream()->len);
    setWidth(128);
    setHeight(128);
    dsp_buffer_set(getStream()->buf, getStream()->len, 0);
    setCorrelationOrder(order);
}

BaselineCollection::~BaselineCollection()
{
    dsp_stream_free_buffer(getStream());
    dsp_stream_free(getStream());
    this->clear();
}

void BaselineCollection::update()
{
    if(getNodes()->count() < 2) return;
    this->clear();
    for(int i = 0; i < getNodes()->count() * (getNodes()->count() - 1) / 2; i++)
    {
        VLBINode** nodes = (VLBINode**)malloc(sizeof(VLBINode*) * getCorrelationOrder());
        char name[150] = "";
        for(int o = 0; o < getCorrelationOrder(); o++) {
            int idx = (i + o * (i / getNodes()->count() + 1)) % getNodes()->count();
            nodes[o] = getNodes()->at(idx);
            if(o == 0)
                sprintf(name, "%s", nodes[o]->getName());
            else
                sprintf(name, "%s_%s", name, nodes[o]->getName());
        }
        VLBIBaseline *b = new VLBIBaseline(nodes, getCorrelationOrder());
        if(!this->contains(name))
            this->add(b);
        else
            b->~VLBIBaseline();
    }
    setRelative(isRelative());
    setRa(getRa());
    setDec(getDec());
}

void BaselineCollection::add(VLBIBaseline * element)
{
    element->getStream()->parent = Stream;
    VLBICollection::add(element, element->getName());
}

void BaselineCollection::remove(const char* name)
{
    VLBICollection::remove(name);
    setCorrelationOrder(getCorrelationOrder());
}

void BaselineCollection::clear()
{
    for(int i = 0; i < count(); i++)
    {
        at(i)->~VLBIBaseline();
    }
    VLBICollection::clear();
}

VLBIBaseline * BaselineCollection::get(const char* name)
{
    return (VLBIBaseline *)VLBICollection::get(name);
}

VLBIBaseline * BaselineCollection::at(int index)
{
    return (VLBIBaseline *)(VLBICollection::at(index));
}

bool BaselineCollection::contains(const char* element)
{
    return VLBICollection::contains(element);
}

void BaselineCollection::setTarget(double *target)
{
    memcpy(Stream->target, target, sizeof(double) * 3);
    for(int i = 0; i < count(); i++)
    {
        at(i)->setTarget(Stream->target);
    }
}

void BaselineCollection:: setRa(double ra)
{
    Ra = ra;
    for(int i = 0; i < count(); i++)
    {
        at(i)->setRa(ra);
    }
}

void BaselineCollection:: setDec(double dec)
{
    Dec = dec;
    for(int i = 0; i < count(); i++)
    {
        at(i)->setDec(dec);
    }
}

void BaselineCollection:: setDistance(double dist)
{
    Dist = dist;
    for(int i = 0; i < count(); i++)
    {
        at(i)->setDistance(dist);
    }
}

void BaselineCollection::setFrequency(double frequency)
{
    Stream->wavelength = vlbi_astro_mean_speed(0) / frequency;
    for(int i = 0; i < count(); i++)
    {
        at(i)->setWaveLength(vlbi_astro_mean_speed(0) / frequency);
    }
}

void BaselineCollection::setRelative(bool rel)
{
    relative = rel;
    for(int i = 0; i < count(); i++)
    {
        at(i)->setRelative(rel);
    }
}

void BaselineCollection::setSampleRate(double samplerate)
{
    Stream->samplerate = samplerate;
    for(int i = 0; i < count(); i++)
    {
        at(i)->setSampleRate(samplerate);
    }
}

void BaselineCollection::setDelegate(vlbi_func2_t delegate)
{
    for(int i = 0; i < count(); i++)
    {
        at(i)->setDelegate(delegate);
    }
}
