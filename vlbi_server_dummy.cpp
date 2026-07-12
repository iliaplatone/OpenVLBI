/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2023  Ilia Platone
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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

Server *VLBI::server = (Server*)new DUMMYServer();
