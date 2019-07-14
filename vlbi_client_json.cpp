#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <vlbi_client_json.h>

void VLBIClient_JSON::AddNode(char* node)
{
    json_object* obj = json_object_new_string (node);
    dsp_stream_p stream = dsp_stream_new();
    dsp_stream_add_dim(stream, len);
    char *base64 = obj->c_string.ptr;
    int len = obj->c_string.str.len * 3 / 4;
    char *buf = (char*)malloc(len);
    buf = from64tobits_fast(buf, base64, obj->c_string.str.len);
    dsp_stream_set_buffer(stream, buf, len);
}

void VLBIClient_JSON::SetFrequency(double centerfrequency)
{
    response.frequency->c_double = stream->frequency;
}

void VLBIClient_JSON::SetSampleRate(double samplingfrequency)
{
    response.samplerate->c_double = samplingfrequency;
}

void VLBIClient_JSON::SetBadwidth(double bandwidth)
{
    response.bandwidth->c_double = bandwidth;
}

void VLBIClient_JSON::SetBPS(int BPS)
{
    response.samplerate->c_double = BPS;
}

