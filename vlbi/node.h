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
    VLBINode(dsp_stream_p stream, int index, bool geographic_coordinates=true);
    ~VLBINode();

    inline char *getName() { return Name; }
    inline dsp_stream_p getStream() { return Stream; }

    inline double* getTarget() { return getStream()->target; }
    inline double getIndex() { return Index; }
    inline double getWaveLength() { return getStream()->wavelength; }
    inline double getSampleRate() { return getStream()->samplerate; }
    inline double* getLocation() { return Location; }
    inline double* getGeographicLocation() {
        GeographicLocation[0] = getLocation()[0];
        GeographicLocation[1] = getLocation()[1];
        GeographicLocation[2] = vlbi_astro_estimate_geocentric_elevation(getLocation()[0], getLocation()[2]);
        return GeographicLocation;
    }
    inline double getStartTime() { return (double)vlbi_time_timespec_to_J2000time(getStream()->starttimeutc); }

    inline void setWaveLength(double wavelength) { getStream()->wavelength = wavelength; }
    inline void setSampleRate(double samplerate) { getStream()->samplerate = samplerate; }
    inline void setStartTime(double starttime) { getStream()->starttimeutc.tv_sec = floor(starttime); getStream()->starttimeutc.tv_nsec = (starttime-getStream()->starttimeutc.tv_sec)*1000000000.0; }

    inline void setTarget(double horiz, double vert) { getStream()->target[0] = horiz; getStream()->target[1] = vert; }
    inline void setTarget(double *target) { getStream()->target = target; }
    inline void setLocation(double *coords) { setLocation(coords[0], coords[1], coords[2]); }
    inline void setLocation(dsp_location location) { setLocation(location.xyz.x, location.xyz.y, location.xyz.z); }
    inline void setLocation(int x) { setLocation(getStream()->location[x].xyz.x, getStream()->location[x].xyz.y, getStream()->location[x].xyz.z); }
    inline void setLocation(double x_or_lat, double y_or_lon, double z_or_el)
    {
        Location[0] = x_or_lat;
        Location[1] = y_or_lon;
        Location[2] = z_or_el;
    }
    inline bool GeographicCoordinates() { return Geo; }
    inline void useGeographicCoordinates(bool geo) { Geo = geo; }
private:
    double GeographicLocation[3];
    double Location[3];
    bool Geo;
    dsp_stream_p Stream;
    char *Name;
    int Index;
};

#endif //_NODE_H
