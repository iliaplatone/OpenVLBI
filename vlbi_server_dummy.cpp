#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_server_dummy.h"

DUMMYServer::DUMMYServer()
    : VLBI::Server::Server()
{
}

DUMMYServer::~DUMMYServer()
{
}

int DUMMYServer::Init(int argc, char** argv)
{
    return VLBI::Server::Init(argc, argv);
}

void DUMMYServer::Parse()
{
    VLBI::Server::Parse();
}

DUMMYServer *client = new DUMMYServer();
