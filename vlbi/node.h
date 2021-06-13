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
#include <cstring>
#include <vlbi.h>

class VLBINode
{
public:
    VLBINode(dsp_stream_p stream, bool geographic_coordinates=true);
    ~VLBINode();

    inline char *getName() { return Name; }
    inline dsp_stream_p getStream() { return Stream; }

    inline double* getTarget() { return getStream()->target; }
    inline double getWaveLength() { return getStream()->wavelength; }
    inline double getSampleRate() { return getStream()->samplerate; }
    inline double* getLocation() { return getStream()->location; }
    inline double getStartTime() { return (double)getStream()->starttimeutc.tv_sec+((double)getStream()->starttimeutc.tv_nsec/1000000000.0); }

    inline void setWaveLength(double wavelength) { getStream()->wavelength = wavelength; }
    inline void setSampleRate(double samplerate) { getStream()->samplerate = samplerate; }
    inline void setStartTime(double starttime) { getStream()->starttimeutc.tv_sec = floor(starttime); getStream()->starttimeutc.tv_nsec = (starttime-getStream()->starttimeutc.tv_sec)*1000000000.0; }

    inline void setTarget(double horiz, double vert) { getStream()->target[0] = horiz; getStream()->target[1] = vert; }
    inline void setTarget(double *target) { getStream()->target = target; }
    inline void setLocation(double *coords) { getStream()->location = coords; }
    inline void setLocation(double x_or_lat, double y_or_lon, double z_or_el)
    {
        if (Geo) {
            double* location = (double*)calloc(sizeof(double), 3);
            location[0] = x_or_lat;
            location[1] = y_or_lon;
            location[2] = z_or_el;
            getStream()->location = vlbi_calc_location(location);
            free(location);
            getStream()->location[2] = vlbi_astro_estimate_geocentric_elevation(x_or_lat, z_or_el);
        } else {
            getStream()->location[0] = x_or_lat;
            getStream()->location[1] = y_or_lon;
            getStream()->location[2] = z_or_el;
        }
    }

private:
    bool Geo;
    dsp_stream_p Stream;
    char *Name;
};

#endif //_NODE_H
