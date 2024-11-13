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
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_server_shared.h"

using namespace VLBI;

class SHAREDServer : public VLBI::Server
{
public:
    SHAREDServer();
    ~SHAREDServer();

    int Init(int argc, char** argv);
    void Parse();
};

SHAREDServer::SHAREDServer()
    : VLBI::Server::Server()
{
}

SHAREDServer::~SHAREDServer()
{
}

int SHAREDServer::Init(int argc, char** argv)
{
    return VLBI::Server::Init(argc, argv);
}

void SHAREDServer::Parse()
{
    VLBI::Server::Parse();
}

static Server *VLBI_Server = (Server*)new SHAREDServer();

void VLBI_Server_Parse() { VLBI_Server->Parse(); }
void VLBI_Server_addContext(const char *name) { VLBI_Server->addContext(name); }
void VLBI_Server_delContext(const char *name) { VLBI_Server->delContext(name); }
void VLBI_Server_setContext(const char *name) { VLBI_Server->setContext(name); }
vlbi_context VLBI_Server_getContext() { return VLBI_Server->getContext(); }
char* VLBI_Server_currentContext() { return VLBI_Server->currentContext(); }
void VLBI_Server_addModel(const char *name, char *format, char *b64) { VLBI_Server->addModel(name, format, b64); }
dsp_stream_p VLBI_Server_getModel(const char *name) { return VLBI_Server->getModel(name); }
char* VLBI_Server_getModel(const char *name, char *format) { return VLBI_Server->getModel(name, format); }
void VLBI_Server_delModel(const char *name) { VLBI_Server->delModel(name); }
int VLBI_Server_getModels(char** names) { return VLBI_Server->getModels(names); }
void VLBI_Server_addNodeFromFits(const char *name, char *b64) { VLBI_Server->addNode(name, b64); }
void VLBI_Server_addNodes(const char *name, char *b64) { VLBI_Server->addNodes(name, b64); }
void VLBI_Server_addNode(const char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo) { VLBI_Server->addNode(name, locations, buf, len, starttime, geo); }
void VLBI_Server_delNode(const char *name) { VLBI_Server->delNode(name); }
void VLBI_Server_CopyNode(const char *name, const char *node) { VLBI_Server->CopyNode(name, node); }
void VLBI_Server_Plot(const char *name, int flags) { VLBI_Server->Plot(name, flags); }
void VLBI_Server_Idft(const char *name, const char *magnitude, const char *phase) { VLBI_Server->Idft(name, magnitude, phase); }
void VLBI_Server_Dft(const char *name, const char *magnitude, const char *phase) { VLBI_Server->Dft(name, magnitude, phase); }
void VLBI_Server_Mask(const char *name, const char *model, const char *mask) { VLBI_Server->Mask(name, model, mask); }
void VLBI_Server_Stack(const char *name, const char *model1, const char *model2) { VLBI_Server->Stack(name, model1, model2); }
void VLBI_Server_Copy(const char *name, const char *model) { VLBI_Server->Copy(name, model); }
void VLBI_Server_Diff(const char *name, const char *model1, const char *model2) { VLBI_Server->Diff(name, model1, model2); }
void VLBI_Server_Convolve(const char *name, const char *model1, const char *model2) { VLBI_Server->Convolve(name, model1, model2); }
void VLBI_Server_LowPass(const char *name, const char *node, double freq) { VLBI_Server->LowPass(name, node, freq); }
void VLBI_Server_HighPass(const char *name, const char *node, double freq) { VLBI_Server->HighPass(name, node, freq); }
void VLBI_Server_BandPass(const char *name, const char *node, double lofreq, double hifreq) { VLBI_Server->BandPass(name, node, lofreq, hifreq); }
void VLBI_Server_BandReject(const char *name, const char *node, double lofreq, double hifreq) { VLBI_Server->BandReject(name, node, lofreq, hifreq); }
void VLBI_Server_Shift(const char *name) { VLBI_Server->Shift(name); }
void VLBI_Server_setRa(double value) { VLBI_Server->setRa(value); }
void VLBI_Server_setDec(double value) { VLBI_Server->setDec(value); }
void VLBI_Server_setFreq(double value) { VLBI_Server->setFreq(value); }
void VLBI_Server_setSampleRate(double value) { VLBI_Server->setSampleRate(value); }
void VLBI_Server_setBps(int value) { VLBI_Server->setBps(value); }
void VLBI_Server_setWidth(int value) { VLBI_Server->setWidth(value); }
void VLBI_Server_setHeight(int value) { VLBI_Server->setHeight(value); }
double VLBI_Server_getRa() { return VLBI_Server->getRa(); }
double VLBI_Server_getDec() { return VLBI_Server->getDec(); }
double VLBI_Server_getFreq() { return VLBI_Server->getFreq(); }
double VLBI_Server_getSampleRate() { return VLBI_Server->getSampleRate(); }
void VLBI_Server_setCorrelationOrder(int order) { VLBI_Server->setCorrelationOrder(order); }
double VLBI_Server_getBps() { return VLBI_Server->getBps(); }
double VLBI_Server_getWidth() { return VLBI_Server->getWidth(); }
double VLBI_Server_getHeight() { return VLBI_Server->getHeight(); }
void VLBI_Server_setInput(FILE* in) { VLBI_Server->setInput(in); }
FILE* VLBI_Server_getInput() { return VLBI_Server->getInput(); }
void VLBI_Server_setOutput(FILE* out) { VLBI_Server->setOutput(out); }
FILE* VLBI_Server_getOutput() { return VLBI_Server->getOutput(); }
void VLBI_Server_setDelegate(vlbi_func2_t func) { VLBI_Server->setDelegate(func); }
vlbi_func2_t VLBI_Server_getDelegate() { return VLBI_Server->getDelegate(); }
