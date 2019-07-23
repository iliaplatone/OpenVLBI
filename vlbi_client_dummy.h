#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <base64.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "vlbi_client.h"


class VLBIClient_DUMMY : public VLBIClient
{
public:
    VLBIClient_DUMMY();
    ~VLBIClient_DUMMY();

    int Init(int argc, char** argv) { return 0; }
    void Parse(char* cmd, char* arg, char* value) { VLBIClient::Parse(cmd, arg, value); }
};
