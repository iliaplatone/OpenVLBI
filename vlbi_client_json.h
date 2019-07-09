#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <base64.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vlbi.h>
#include <vlbi_client.h>

typedef struct json_response {
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
    VLBIClient_JSON();
    ~VLBIClient_JSON();
    
    virtual void AddNode(char* node);
    virtual void DelNode(char* name);
    virtual void SetCapture(double seconds);
    virtual void AbortCapture();
    virtual void SetExposure(double seconds);
    virtual void AbortExposure();
    virtual void SetFrequency(double centerfrequency);
    virtual void SetSampleRate(double samplingfrequency);
    virtual void SetBadwidth(double bandwidth);
    virtual void SetGain(double gain);
    virtual void SetBPS(int BPS);

private:
    json_response* response;
};
