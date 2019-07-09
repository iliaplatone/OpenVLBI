#include <vlbi_client.h>
#include <json-glib.h>

public VLBIClient_JSON::VLBIClient_JSON()
  : VLBIClient()
{
    CreateContext();
}

public VLBIClient_JSON::~VLBIClient_JSON()
{
    FreeContext();
}

void VLBIClient_JSON::AddNode(char* node)
{
    int len = strlen(buf) * 3 / 4;
    JsonNode node = json_from_string (node);
    dsp_stream *stream = malloc(sizeof(dsp_stream));
    char *base64 = json_node_get_string (node);
    char *buf = malloc(len);
    buf = from64tobits_fast(buf, base64, strlen(buf));
    dsp_stream_set_buffer(stream, buf, len);
}

void VLBIClient_JSON::SetFrequency(double centerfrequency)
{
    response->frequency = json_node_set_double(response->frequency, stream->frequency);
}

void VLBIClient_JSON::SetSampleRate(double samplingfrequency)
{
    response->samplerate = json_node_set_double(response->samplerate, stream->samplerate);
}

void VLBIClient_JSON::SetBadwidth(double bandwidth)
{
    response->bandwidth = json_node_set_double(response->bandwidth, stream->bandwidth);
}

void VLBIClient_JSON::SetBPS(int BPS)
{
    response->bitspersample = json_node_set_double(response->bitspersample, stream->bitspersample);
}

