#ifndef _VLBI_CLIENT_JSON_H
#define _VLBI_CLIENT_JSON_H

#include <vlbi_client.h>
#include <json.h>

struct json_response {
    json_object *name;
    struct json_object *plot;
    struct json_object *capturetime;
    struct json_object *start_capture;
    struct json_object *samplerate;
    struct json_object *frequency;
    struct json_object *bandwidth;
    struct json_object *gain;
    struct json_object *depth;
    struct json_object *ra;
    struct json_object *dec;
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

    ~VLBIClient_JSON() {}

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
