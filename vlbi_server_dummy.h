#ifndef VLBI_SERVER_DUMMY_H
#define VLBI_SERVER_DUMMY_H

#include "vlbi_server.h"

using namespace VLBI;

class DUMMYServer : public VLBI::Server
{
    public:
        DUMMYServer();
        ~DUMMYServer();

        int Init(int argc, char** argv);
        void Parse();
};

#endif //VLBI_SERVER_DUMMY_H
