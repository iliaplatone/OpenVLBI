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


BaselineCollection::BaselineCollection(NodeCollection *nodes, vlbi_func2_t correlation_func, bool m, double u, double v) : VLBICollection::VLBICollection()
{
    Stream = dsp_stream_new();
    dsp_stream_add_dim(Stream, u);
    dsp_stream_add_dim(Stream, v);
    dsp_stream_alloc_buffer(Stream, Stream->len);
    dsp_buffer_clear(Stream);
    for(int i = 0; i < nodes->Count; i++)
    {
        for(int l = i + 1; l < nodes->Count; l++)
        {
            dsp_stream_p node1 = nodes->At(i)->getStream();
            dsp_stream_p node2 = nodes->At(l)->getStream();
            this->Add(new VLBIBaseline(node1, node2, correlation_func, m));
        }
    }
}

BaselineCollection::~BaselineCollection()
{
    for(int i = 0; i < Count; i++) {
        At(i)->~VLBIBaseline();
    }
}

void BaselineCollection::Add(VLBIBaseline * element, const char *name)
{
    element->getStream()->parent = Stream;
    VLBICollection::Add(element, name);
    fprintf(stderr, "added baseline %s\n", (char*)element->getStream()->arg);
}

void BaselineCollection::Remove(const char* name)
{
    VLBICollection::Remove(name);
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
        At(i)->setTarget(target);
    }
}

void BaselineCollection::SetFrequency(double frequency)
{
    Stream->lambda = LIGHTSPEED / frequency;
    for(int i = 0; i < Count; i++) {
        At(i)->setWaveLength(LIGHTSPEED / frequency);
    }
}

void BaselineCollection::SetSampleRate(double samplerate)
{
    Stream->samplerate = samplerate;
    for(int i = 0; i < Count; i++) {
        At(i)->setSampleRate(samplerate);
    }
}
