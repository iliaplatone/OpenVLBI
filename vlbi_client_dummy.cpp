#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_client_dummy.h"

DUMMYClient::DUMMYClient()
	: VLBI::Client::Client()
{
}

DUMMYClient::~DUMMYClient()
{
}

int DUMMYClient::Init(int argc, char** argv)
{
	return VLBI::Client::Init(argc, argv);
}

void DUMMYClient::Parse()
{
    VLBI::Client::Parse();
}

DUMMYClient *client = new DUMMYClient();
