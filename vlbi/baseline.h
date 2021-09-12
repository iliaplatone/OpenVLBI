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
#include <node.h>

class VLBIBaseline
{
public:
    VLBIBaseline(VLBINode *node1, VLBINode *node2);
    ~VLBIBaseline();

    inline char *getName() { return Name; }

    double Correlate(double time);
    double Correlate(double time1, double time2);
    double Correlate(int idx1, int idx2);
    double getStartTime();
    double getEndTime();

    double *getBaseline();
    void getProjection();

    inline double getX() { return baseline[0]; }
    inline double getY() { return baseline[1]; }
    inline double getZ() { return baseline[2]; }
    inline double getU() { return u; }
    inline double getV() { return v; }
    inline double getDelay() { return delay; }
    void setTime(double time);

    inline double getRa() { return Ra; }
    inline double getDec() { return Dec; }
    inline double* getTarget() { return Target; }
    inline double getWaveLength() { return WaveLength; }
    inline double getSampleRate() { return SampleRate; }

    inline void setRa(double ra) { Ra = ra; }
    inline void setDec(double dec) { Dec = dec; }
    inline void setTarget(double horiz, double vert) { Target[0] = horiz; Target[1] = vert; memcpy(getStream()->target, Target, sizeof(double)*2); getNode1()->setTarget(Target); getNode2()->setTarget(Target); }
    inline void setTarget(double *target) { memcpy(Target, target, sizeof(double)*2); memcpy(getStream()->target, Target, sizeof(double)*2); getNode1()->setTarget(Target); getNode2()->setTarget(Target); }
    inline void setWaveLength(double wavelength) { WaveLength = wavelength; getStream()->wavelength = wavelength; getNode1()->setWaveLength(WaveLength); getNode2()->setWaveLength(WaveLength); }
    inline void setSampleRate(double samplerate) { SampleRate = samplerate; getStream()->samplerate = samplerate; getNode1()->setSampleRate(SampleRate); getNode2()->setSampleRate(SampleRate); }

    VLBINode* getNode1() { return Node1; }
    VLBINode* getNode2() { return Node2; }
    dsp_stream_p getStream() { return Stream; }
    void setDelegate(vlbi_func2_t delegate) { dsp_correlation_delegate = delegate; }

    inline bool Locked() { return locked; }
    inline void Lock() { locked = true; }
    inline void Unlock() { locked = false; }
    inline void setRelative(bool rel) { relative = rel; }
    inline bool isRelative() { return relative; }
    inline dsp_location* stationLocation() { return &station; }

private:
    dsp_location station;
    bool relative { false };
    bool locked { false };
    double Target[3];
    double Ra { 0 };
    double Dec { 0 };
    double baseline[3];
    double u { 0 };
    double v { 0 };
    double delay { 0 };
    double WaveLength { 0 };
    double SampleRate { 0 };
    int max_threads { 0 };
    VLBINode* Node1;
    VLBINode* Node2;
    vlbi_func2_t dsp_correlation_delegate;
    char *Name;
    dsp_stream_p Stream;
};

#endif //_BASELINE_H
