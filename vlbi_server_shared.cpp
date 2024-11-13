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
#include "vlbi_server.h"
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

Server *VLBI::server = (Server*)new SHAREDServer();

int VLBI_Server_Init(int argc, char** argv) { return VLBI::server->Init(argc, argv); }
void VLBI_Server_Parse() { VLBI::server->Parse(); }
void VLBI_Server_addContext(const char *name) { VLBI::server->addContext(name); }
void VLBI_Server_delContext(const char *name) { VLBI::server->delContext(name); }
void VLBI_Server_setContext(const char *name) { VLBI::server->setContext(name); }
vlbi_context VLBI_Server_getContext() { return VLBI::server->getContext(); }
char* VLBI_Server_currentContext() { return VLBI::server->currentContext(); }
void VLBI_Server_addModel(const char *name, char *format, char *b64) { VLBI::server->addModel(name, format, b64); }
dsp_stream_p VLBI_Server_getModel(const char *name) { return VLBI::server->getModel(name); }
char* VLBI_Server_getModel(const char *name, char *format) { return VLBI::server->getModel(name, format); }
void VLBI_Server_delModel(const char *name) { VLBI::server->delModel(name); }
int VLBI_Server_getModels(char** names) { return VLBI::server->getModels(names); }
void VLBI_Server_addNodeFromFits(const char *name, char *b64) { VLBI::server->addNode(name, b64); }
void VLBI_Server_addNodes(const char *name, char *b64) { VLBI::server->addNodes(name, b64); }
void VLBI_Server_addNode(const char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo) { VLBI::server->addNode(name, locations, buf, len, starttime, geo); }
void VLBI_Server_delNode(const char *name) { VLBI::server->delNode(name); }
void VLBI_Server_CopyNode(const char *name, const char *node) { VLBI::server->CopyNode(name, node); }
void VLBI_Server_Plot(const char *name, int flags) { VLBI::server->Plot(name, flags); }
void VLBI_Server_Idft(const char *name, const char *magnitude, const char *phase) { VLBI::server->Idft(name, magnitude, phase); }
void VLBI_Server_Dft(const char *name, const char *magnitude, const char *phase) { VLBI::server->Dft(name, magnitude, phase); }
void VLBI_Server_Mask(const char *name, const char *model, const char *mask) { VLBI::server->Mask(name, model, mask); }
void VLBI_Server_Stack(const char *name, const char *model1, const char *model2) { VLBI::server->Stack(name, model1, model2); }
void VLBI_Server_Copy(const char *name, const char *model) { VLBI::server->Copy(name, model); }
void VLBI_Server_Diff(const char *name, const char *model1, const char *model2) { VLBI::server->Diff(name, model1, model2); }
void VLBI_Server_Convolve(const char *name, const char *model1, const char *model2) { VLBI::server->Convolve(name, model1, model2); }
void VLBI_Server_LowPass(const char *name, const char *node, double freq) { VLBI::server->LowPass(name, node, freq); }
void VLBI_Server_HighPass(const char *name, const char *node, double freq) { VLBI::server->HighPass(name, node, freq); }
void VLBI_Server_BandPass(const char *name, const char *node, double lofreq, double hifreq) { VLBI::server->BandPass(name, node, lofreq, hifreq); }
void VLBI_Server_BandReject(const char *name, const char *node, double lofreq, double hifreq) { VLBI::server->BandReject(name, node, lofreq, hifreq); }
void VLBI_Server_Shift(const char *name) { VLBI::server->Shift(name); }
void VLBI_Server_setRa(double value) { VLBI::server->setRa(value); }
void VLBI_Server_setDec(double value) { VLBI::server->setDec(value); }
void VLBI_Server_setFreq(double value) { VLBI::server->setFreq(value); }
void VLBI_Server_setSampleRate(double value) { VLBI::server->setSampleRate(value); }
void VLBI_Server_setBps(int value) { VLBI::server->setBps(value); }
void VLBI_Server_setWidth(int value) { VLBI::server->setWidth(value); }
void VLBI_Server_setHeight(int value) { VLBI::server->setHeight(value); }
double VLBI_Server_getRa() { return VLBI::server->getRa(); }
double VLBI_Server_getDec() { return VLBI::server->getDec(); }
double VLBI_Server_getFreq() { return VLBI::server->getFreq(); }
double VLBI_Server_getSampleRate() { return VLBI::server->getSampleRate(); }
void VLBI_Server_setCorrelationOrder(int order) { VLBI::server->setCorrelationOrder(order); }
double VLBI_Server_getBps() { return VLBI::server->getBps(); }
double VLBI_Server_getWidth() { return VLBI::server->getWidth(); }
double VLBI_Server_getHeight() { return VLBI::server->getHeight(); }
void VLBI_Server_setInput(FILE* in) { VLBI::server->setInput(in); }
FILE* VLBI_Server_getInput() { return VLBI::server->getInput(); }
void VLBI_Server_setOutput(FILE* out) { VLBI::server->setOutput(out); }
FILE* VLBI_Server_getOutput() { return VLBI::server->getOutput(); }
void VLBI_Server_setDelegate(vlbi_func2_t func) { VLBI::server->setDelegate(func); }
vlbi_func2_t VLBI_Server_getDelegate() { return VLBI::server->getDelegate(); }
