#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_client_dummy.h"

DUMMYClient::DUMMYClient()
{
}

DUMMYClient::~DUMMYClient()
{
}

int DUMMYClient::Init(int argc, char** argv)
{
	return VLBI::Client::Init(argc, argv);
}

void DUMMYClient::Parse(char* cmd, char* arg, char* value)
{
	VLBI::Client::Parse(cmd, arg, value);
}

DUMMYClient *client;
