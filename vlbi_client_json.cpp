#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_client_json.h"

JSONClient::JSONClient()
{
}

JSONClient::~JSONClient()
{
}

int JSONClient::Init(int argc, char** argv)
{
	return VLBI::Client::Init(argc, argv);
}

void JSONClient::Parse(char* cmd, char* arg, char* value)
{
	VLBI::Client::Parse(cmd, arg, value);
}

JSONClient *client;
