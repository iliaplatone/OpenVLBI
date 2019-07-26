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

#ifndef _NODE_H
#define _NODE_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vlbi.h>

class VLBINode
{
public:
    VLBINode(dsp_stream_p stream);
    ~VLBINode();

    void setCoordinates(double x, double y, double z);
    void setCoordinates(double lat, double lon, double el, bool osl);
    dsp_stream_p getStream() { return Stream; }

private:
    dsp_stream_p Stream;
    char *Name;
};

#endif //_NODE_H
