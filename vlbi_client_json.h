#ifndef VLBI_CLIENT_JSON_H
#define VLBI_CLIENT_JSON_H

#include "vlbi_client.h"

using namespace VLBI;

class JSONClient : public VLBI::Client
{
public:
    JSONClient();
    ~JSONClient();

    int Init(int argc, char** argv);
    void Parse(FILE* f);

private:
    bool nodelay;
    bool relative;
    vlbi_node node;
};

#endif //VLBI_CLIENT_JSON_H
