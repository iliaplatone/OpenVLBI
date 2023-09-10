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
    VLBIBaseline(VLBINode **nodes, int nodes_count);
    VLBIBaseline(VLBINode *node1, VLBINode *node2);
    ~VLBIBaseline();

    inline char *getName() { return Name; }

    double Correlate(double time);
    double Correlate(double time1, double time2);
    double Correlate(int idx1, int idx2);
    double Correlate(double *times);
    double Correlate(int *indexes);
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
    inline double getDistance() { return Dist; }
    inline double* getTarget() { return Target; }
    inline double getWaveLength() { return WaveLength; }
    inline double getSampleRate() { return SampleRate; }

    inline void setRa(double ra) { Ra = ra; }
    inline void setDec(double dec) { Dec = dec; }
    inline void setDistance(double dist) { Dist = dist; }
    inline void setTarget(double horiz, double vert, double dist = DBL_MAX) { Target[0] = horiz; Target[1] = vert; Target[2] = dist; memcpy(getStream()->target, Target, sizeof(double)*3); for(int i = 0; i < nodes_count; i++) getNode(i)->setTarget((Target)); }
    inline void setTarget(double *target) { memcpy(Target, target, sizeof(double)*3); memcpy(getStream()->target, Target, sizeof(double)*3); for(int i = 0; i < nodes_count; i++) getNode(i)->setTarget((Target)); }
    inline void setWaveLength(double wavelength) { WaveLength = wavelength; getStream()->wavelength = wavelength; for(int i = 0; i < nodes_count; i++) getNode(i)->setWaveLength(WaveLength); }
    inline void setSampleRate(double samplerate) { SampleRate = samplerate; getStream()->samplerate = samplerate; for(int i = 0; i < nodes_count; i++) getNode(i)->setSampleRate(SampleRate); }

    inline VLBINode* getNode(int index) { return Nodes[index]; }
    inline dsp_stream_p getStream() { return Stream; }
    inline void setStream(dsp_stream_p stream)
    {
        Stream = stream;
        if(getStream()->dims < 2)
            dsp_stream_add_dim(getStream(), 1);
        if(Stream->magnitude == nullptr) {
            Stream->magnitude = dsp_stream_copy(Stream);
            dsp_buffer_set(Stream->magnitude->buf, Stream->magnitude->len, 0.0);
        }
        if(Stream->phase == nullptr) {
            Stream->phase = dsp_stream_copy(Stream);
            dsp_buffer_set(Stream->phase->buf, Stream->phase->len, 0.0);
        }
    }
    inline void freeStream() {
        dsp_stream_free_buffer(getStream());
        dsp_stream_free(getStream());
    }
    inline void setDelegate(vlbi_func2_t delegate) { dsp_correlation_delegate = delegate; }

    inline bool Locked() { return locked; }
    inline void Lock() { locked = true; }
    inline void Unlock() { locked = false; }
    inline void setRelative(bool rel) { relative = rel; }
    inline bool isRelative() { return relative; }
    inline dsp_location* stationLocation() { return &station; }
    inline complex_t *getBufferData();
private:
    complex_t* dft;
    dsp_location station;
    bool relative { false };
    bool locked { false };
    double Target[3];
    double Ra { 0 };
    double Dec { 0 };
    double Dist { DBL_MAX };
    double baseline[3];
    double u { 0 };
    double v { 0 };
    double delay { 0 };
    double WaveLength { 0 };
    double SampleRate { 0 };
    int max_threads { 0 };
    VLBINode** Nodes;
    int nodes_count;
    vlbi_func2_t dsp_correlation_delegate;
    char *Name;
    dsp_stream_p Stream;
};

#endif //_BASELINE_H
