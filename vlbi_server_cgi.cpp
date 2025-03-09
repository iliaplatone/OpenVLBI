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
#include "vlbi_server_cgi.h"
#include "QtCGI.h"

CGIServer::CGIServer()
{
}

CGIServer::~CGIServer()
{
    readThread->stop();
    readThread->wait();
    readThread->~Thread();
    uiThread->stop();
    uiThread->wait();
    uiThread->~Thread();
    app->exit();
}

int CGIServer::Init(int argc, char** argv)
{
    app = new QCoreApplication(argc, argv);
    readThread = new Thread(app);
    uiThread = new Thread(app, 1000, 1000);
    app->connect(readThread, static_cast<void (Thread::*)(Thread*)>(&Thread::threadLoop), [ = ] (Thread * thread)
     {
        char buf;
        fread(&buf, 1, 1, getOutput());
        response.append(buf);
        thread->unlock();
    });
    app->connect(uiThread, static_cast<void (Thread::*)(Thread*)>(&Thread::threadLoop), [ = ] (Thread * thread)
    {
        QString header = "<form action=\"vlbi_server_cgi\" method=\"post\"><input name=\"command\" placeholder=\"enter command\" type=\"text\"><input type=\"submit\" value=\"exec\"></form>";
        QtCGI::Instance()->SetContent(header+"<div>"+response+"</div>");
        QtCGI::Instance()->WriteResponse();
        response.clear();
        thread->unlock();
    });
    readThread->start();
    uiThread->start();
    return JSONServer::Init(argc, argv);
}

void CGIServer::Parse()
{
    if (!QtCGI::Instance()->GetParam("command").isEmpty()) {
        QString request = QtCGI::Instance()->GetParam("command");
        fwrite(request.toUtf8(), request.length(), 1, getInput());
    }
    JSONServer::Parse();
}

Server *VLBI::server = (Server*)new CGIServer();
