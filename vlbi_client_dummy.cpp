#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include "vlbi_client_dummy.h"

VLBIClient_DUMMY::VLBIClient_DUMMY()
	:VLBIClient()
{
}

VLBIClient_DUMMY::~VLBIClient_DUMMY()
{
}

VLBIClient_DUMMY *client;
