#include "vlbi_server_dummy.h"
#include "vlbi_server_shared.h"

using namespace VLBI;

typedef void* vlbi_server_instance;

vlbi_server_instance VLBI_Server_Create() { DUMMYServer *server = new DUMMYServer(); return static_cast<vlbi_server_instance>(server); }
void VLBI_Server_Destroy(vlbi_server_instance instance) { (static_cast<DUMMYServer*>(instance))->~DUMMYServer(); }
void VLBI_Server_Parse(vlbi_server_instance instance) { (static_cast<DUMMYServer*>(instance))->Parse(); }
void VLBI_Server_addContext(vlbi_server_instance instance, const char *name) { (static_cast<DUMMYServer*>(instance))->addContext(name); }
void VLBI_Server_delContext(vlbi_server_instance instance, const char *name) { (static_cast<DUMMYServer*>(instance))->delContext(name); }
void VLBI_Server_setContext(vlbi_server_instance instance, const char *name) { (static_cast<DUMMYServer*>(instance))->setContext(name); }
vlbi_context VLBI_Server_getContext(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getContext(); }
char* VLBI_Server_currentContext(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->currentContext(); }
void VLBI_Server_addModel(vlbi_server_instance instance, const char *name, char *format, char *b64) { (static_cast<DUMMYServer*>(instance))->addModel(name, format, b64); }
dsp_stream_p VLBI_Server_getModel(vlbi_server_instance instance, const char *name) { return (static_cast<DUMMYServer*>(instance))->getModel(name); }
char* VLBI_Server_getModel(vlbi_server_instance instance, const char *name, char *format) { return (static_cast<DUMMYServer*>(instance))->getModel(name, format); }
void VLBI_Server_delModel(vlbi_server_instance instance, const char *name) { (static_cast<DUMMYServer*>(instance))->delModel(name); }
int VLBI_Server_getModels(vlbi_server_instance instance, char** names) { return (static_cast<DUMMYServer*>(instance))->getModels(names); }
void VLBI_Server_addNodeFromFits(vlbi_server_instance instance, const char *name, char *b64) { (static_cast<DUMMYServer*>(instance))->addNode(name, b64); }
void VLBI_Server_addNodes(vlbi_server_instance instance, const char *name, char *b64) { (static_cast<DUMMYServer*>(instance))->addNodes(name, b64); }
void VLBI_Server_addNode(vlbi_server_instance instance, const char *name, dsp_location *locations, void *buf, int len, timespec starttime, bool geo) { (static_cast<DUMMYServer*>(instance))->addNode(name, locations, buf, len, starttime, geo); }
void VLBI_Server_delNode(vlbi_server_instance instance, const char *name) { (static_cast<DUMMYServer*>(instance))->delNode(name); }
void VLBI_Server_CopyNode(vlbi_server_instance instance, const char *name, const char *node) { (static_cast<DUMMYServer*>(instance))->CopyNode(name, node); }
void VLBI_Server_Plot(vlbi_server_instance instance, const char *name, int flags) { (static_cast<DUMMYServer*>(instance))->Plot(name, flags); }
void VLBI_Server_Idft(vlbi_server_instance instance, const char *name, const char *magnitude, const char *phase) { (static_cast<DUMMYServer*>(instance))->Idft(name, magnitude, phase); }
void VLBI_Server_Dft(vlbi_server_instance instance, const char *name, const char *magnitude, const char *phase) { (static_cast<DUMMYServer*>(instance))->Dft(name, magnitude, phase); }
void VLBI_Server_Mask(vlbi_server_instance instance, const char *name, const char *model, const char *mask) { (static_cast<DUMMYServer*>(instance))->Mask(name, model, mask); }
void VLBI_Server_Stack(vlbi_server_instance instance, const char *name, const char *model1, const char *model2) { (static_cast<DUMMYServer*>(instance))->Stack(name, model1, model2); }
void VLBI_Server_Copy(vlbi_server_instance instance, const char *name, const char *model) { (static_cast<DUMMYServer*>(instance))->Copy(name, model); }
void VLBI_Server_Diff(vlbi_server_instance instance, const char *name, const char *model1, const char *model2) { (static_cast<DUMMYServer*>(instance))->Diff(name, model1, model2); }
void VLBI_Server_Convolve(vlbi_server_instance instance, const char *name, const char *model1, const char *model2) { (static_cast<DUMMYServer*>(instance))->Convolve(name, model1, model2); }
void VLBI_Server_LowPass(vlbi_server_instance instance, const char *name, const char *node, double freq) { (static_cast<DUMMYServer*>(instance))->LowPass(name, node, freq); }
void VLBI_Server_HighPass(vlbi_server_instance instance, const char *name, const char *node, double freq) { (static_cast<DUMMYServer*>(instance))->HighPass(name, node, freq); }
void VLBI_Server_BandPass(vlbi_server_instance instance, const char *name, const char *node, double lofreq, double hifreq) { (static_cast<DUMMYServer*>(instance))->BandPass(name, node, lofreq, hifreq); }
void VLBI_Server_BandReject(vlbi_server_instance instance, const char *name, const char *node, double lofreq, double hifreq) { (static_cast<DUMMYServer*>(instance))->BandReject(name, node, lofreq, hifreq); }
void VLBI_Server_Shift(vlbi_server_instance instance, const char *name) { (static_cast<DUMMYServer*>(instance))->Shift(name); }
void VLBI_Server_setRa(vlbi_server_instance instance, double value) { (static_cast<DUMMYServer*>(instance))->setRa(value); }
void VLBI_Server_setDec(vlbi_server_instance instance, double value) { (static_cast<DUMMYServer*>(instance))->setDec(value); }
void VLBI_Server_setFreq(vlbi_server_instance instance, double value) { (static_cast<DUMMYServer*>(instance))->setFreq(value); }
void VLBI_Server_setSampleRate(vlbi_server_instance instance, double value) { (static_cast<DUMMYServer*>(instance))->setSampleRate(value); }
void VLBI_Server_setBps(vlbi_server_instance instance, int value) { (static_cast<DUMMYServer*>(instance))->setBps(value); }
void VLBI_Server_setWidth(vlbi_server_instance instance, int value) { (static_cast<DUMMYServer*>(instance))->setWidth(value); }
void VLBI_Server_setHeight(vlbi_server_instance instance, int value) { (static_cast<DUMMYServer*>(instance))->setHeight(value); }
double VLBI_Server_getRa(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getRa(); }
double VLBI_Server_getDec(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getDec(); }
double VLBI_Server_getFreq(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getFreq(); }
double VLBI_Server_getSampleRate(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getSampleRate(); }
void VLBI_Server_setCorrelationOrder(vlbi_server_instance instance, int order) { (static_cast<DUMMYServer*>(instance))->setCorrelationOrder(order); }
double VLBI_Server_getBps(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getBps(); }
double VLBI_Server_getWidth(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getWidth(); }
double VLBI_Server_getHeight(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getHeight(); }
void VLBI_Server_setInput(vlbi_server_instance instance, FILE* in) { (static_cast<DUMMYServer*>(instance))->setInput(in); }
FILE* VLBI_Server_getInput(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getInput(); }
void VLBI_Server_setOutput(vlbi_server_instance instance, FILE* out) { (static_cast<DUMMYServer*>(instance))->setOutput(out); }
FILE* VLBI_Server_getOutput(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getOutput(); }
void VLBI_Server_setDelegate(vlbi_server_instance instance, vlbi_func2_t func) { (static_cast<DUMMYServer*>(instance))->setDelegate(func); }
vlbi_func2_t VLBI_Server_getDelegate(vlbi_server_instance instance) { return (static_cast<DUMMYServer*>(instance))->getDelegate(); }
