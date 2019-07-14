#ifndef _VLBI_CLIENT_JSON_H
#define _VLBI_CLIENT_JSON_H

#include <vlbi_client.h>
#include <json.h>
#include <json_c_version.h>

struct json_response {
    json_object *name;
    json_object *plot;
    json_object *capturetime;
    json_object *start_capture;
    json_object *samplerate;
    json_object *frequency;
    json_object *bandwidth;
    json_object *gain;
    json_object *depth;
    json_object *ra;
    json_object *dec;
};

class VLBIClient_JSON : public VLBIClient
{
public:
	VLBIClient_JSON()
	  : VLBIClient()
{
    response.name = json_object_new_object();
    response.plot = json_object_new_object();
    response.capturetime = json_object_new_object();
    response.start_capture = json_object_new_object();
    response.samplerate = json_object_new_object();
    response.frequency = json_object_new_object();
    response.bandwidth = json_object_new_object();
    response.gain = json_object_new_object();
    response.depth = json_object_new_object();
    response.ra = json_object_new_object();
    response.dec = json_object_new_object();
}

    VLBIClient_JSON() {}

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
