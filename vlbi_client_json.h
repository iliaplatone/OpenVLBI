
#ifndef _VLBI_CLIENT_JSON_H
#define _VLBI_CLIENT_JSON_H

#include <glib-object.h>
#include <vlbi_client.h>
#include <json-glib/json-glib.h>

struct json_response {
    JsonNode *name;
    JsonNode *plot;
    JsonNode *capturetime;
    JsonNode *start_capture;
    JsonNode *samplerate;
    JsonNode *frequency;
    JsonNode *bandwidth;
    JsonNode *gain;
    JsonNode *depth;
    JsonNode *ra;
    JsonNode *dec;
};

class VLBIClient_JSON : public VLBIClient
{
public:
	VLBIClient_JSON()
	  : VLBIClient::VLBIClient()
{
    response.name = json_node_alloc ();
    response.plot = json_node_alloc ();
    response.capturetime = json_node_alloc ();
    response.start_capture = json_node_alloc ();
    response.samplerate = json_node_alloc ();
    response.frequency = json_node_alloc ();
    response.bandwidth = json_node_alloc ();
    response.gain = json_node_alloc ();
    response.depth = json_node_alloc ();
    response.ra = json_node_alloc ();
    response.dec = json_node_alloc ();
}

    ~VLBIClient_JSON() { }

    void AddNode(char* node);
    void DelNode(char* name);
    void SetCapture(double seconds);
    void AbortCapture();
    void SetExposure(double seconds);
    void AbortExposure();
    void SetFrequency(double centerfrequency);
    void SetSampleRate(double samplingfrequency);
    void SetBadwidth(double bandwidth);
    void SetGain(double gain);
    void SetBPS(int BPS);

private:
    json_response response;
};

#endif
