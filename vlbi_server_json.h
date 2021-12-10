#ifndef VLBI_SERVER_JSON_H
#define VLBI_SERVER_JSON_H

#include "vlbi_server.h"

using namespace VLBI;

class JSONServer : public VLBI::Server
{
    public:
        JSONServer();
        ~JSONServer() override;

        int Init(int argc, char** argv) override;
        void Parse() override;

    private:
        bool nodelay;
        bool relative;
        vlbi_node node;
};

#endif //VLBI_SERVER_JSON_H
