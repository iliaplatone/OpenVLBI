#ifndef VLBI_CLIENT_DUMMY_H
#define VLBI_CLIENT_DUMMY_H

#include "vlbi_client.h"

using namespace VLBI;

class DUMMYClient : public VLBI::Client
{
public:
    DUMMYClient();
    ~DUMMYClient();

    int Init(int argc, char** argv);
    void Parse(char* cmd, char* arg, char* value);
};

#endif //VLBI_CLIENT_DUMMY_H
