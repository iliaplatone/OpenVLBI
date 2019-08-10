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

#ifndef _BASELINE_H
#define _BASELINE_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vlbi.h>

class VLBIBaseline
{
public:
    VLBIBaseline(dsp_stream_p node1, dsp_stream_p node2, bool m=false);
    ~VLBIBaseline();

    double* Correlate(double* buf, double timeoffset, int *idx);
    double* Correlate(double* buf, int index);
    double getUVSize();
    double *getUVCoords(double timeoffset);
    double *getUVCoords();
    double *getUVCoords(int index);
    double *getProjection(double timeoffset);
    double *getBaselineM();
    double *getBaselineCenter();
    double *getBaselineRad();
    void setTarget(double *target);
    void setWaveLength(double wavelength);
    void setSampleRate(double samplerate);
    dsp_stream_p getStream() { return Stream; }
    int max_threads;
    dsp_stream_p first;
    dsp_stream_p second;

private:
    vlbi_func2_t dsp_correlation_delegate;
    double *baseline_rad;
    double *baseline_center;
    double *baseline;
    double HA;
    double deltatime;
    double starttime;
    dsp_stream_p Stream;
    char *Name;
};

#endif //_BASELINE_H
