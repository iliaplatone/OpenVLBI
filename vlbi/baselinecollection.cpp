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

#include <cstdio>
#include <cstdlib>
#include "nodecollection.h"
#include "baseline.h"
#include "baselinecollection.h"


BaselineCollection::BaselineCollection(NodeCollection *nodes) : VLBICollection::VLBICollection()
{
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
    for(int i = 0; i < Count; i++) {
        At(i)->~VLBIBaseline();
    }
}

void BaselineCollection::Update()
{
    this->Clear();
    for(int i = 0; i < getNodes()->Count; i++)
    {
        for(int l = i + 1; l < getNodes()->Count; l++)
        {
            VLBINode* node1 = getNodes()->At(i);
            VLBINode* node2 = getNodes()->At(l);
            VLBIBaseline *b = new VLBIBaseline(node1, node2);
            b->getStream()->parent = Stream;
            this->Add(b);
        }
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

void BaselineCollection::RemoveKey(const char* name)
{
    VLBICollection::RemoveKey(name);
}

void BaselineCollection::Clear()
{
    for(int i = Count-1; i >= 0; i--)
        VLBICollection::RemoveAt(i);
}

VLBIBaseline * BaselineCollection::Get(const char* name)
{
    return (VLBIBaseline *)VLBICollection::Get(name);
}

void BaselineCollection::Remove(VLBIBaseline *element)
{
    VLBICollection::Remove(element);
}

void BaselineCollection::RemoveAt(int index)
{
	VLBICollection::RemoveAt(index);
}

VLBIBaseline * BaselineCollection::At(int index)
{
    return (VLBIBaseline *)(VLBICollection::At(index));
}

bool BaselineCollection::Contains(VLBIBaseline *element)
{
    return VLBICollection::Contains(element);
}

bool BaselineCollection::ContainsKey(const char* element)
{
    return VLBICollection::ContainsKey(element);
}

int BaselineCollection::IndexOf(VLBIBaseline *element)
{
    return VLBICollection::IndexOf(element);
}

void BaselineCollection::SetTarget(double *target)
{
    memcpy(Stream->target, target, sizeof(double) * 3);
    for(int i = 0; i < Count; i++) {
        At(i)->setTarget(Stream->target);
    }
}

void BaselineCollection:: setRa(double ra)
{
    Ra = ra;
    for(int i = 0; i < Count; i++) {
        At(i)->setRa(ra);
    }
}

void BaselineCollection:: setDec(double dec)
{
    Dec = dec;
    for(int i = 0; i < Count; i++) {
        At(i)->setDec(dec);
    }
}

void BaselineCollection::SetFrequency(double frequency)
{
    Stream->wavelength = LIGHTSPEED / frequency;
    for(int i = 0; i < Count; i++) {
        At(i)->setWaveLength(LIGHTSPEED / frequency);
    }
}

void BaselineCollection::setRelative(bool rel)
{
    relative = rel;
    for(int i = 0; i < Count; i++) {
        At(i)->setRelative(rel);
    }
}

void BaselineCollection::SetSampleRate(double samplerate)
{
    Stream->samplerate = samplerate;
    for(int i = 0; i < Count; i++) {
        At(i)->setSampleRate(samplerate);
    }
}

void BaselineCollection::SetDelegate(vlbi_func2_t delegate)
{
    for(int i = 0; i < Count; i++) {
        At(i)->setDelegate(delegate);
    }
}
