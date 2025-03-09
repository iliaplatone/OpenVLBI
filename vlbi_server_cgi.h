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

#ifndef VLBI_SERVER_CGI_H
#define VLBI_SERVER_CGI_H

#include "vlbi_server_json.h"
#include "QtCGI.h"
#include <QObject>
#include <QWidget>
#include <QCoreApplication>
#include "threads.h"

using namespace VLBI;

class CGIServer : public VLBI::JSONServer
{
    public:
        CGIServer();
        ~CGIServer() override;

        int Init(int argc, char** argv) override;
        void Parse() override;

    private:
        QString response;
        Thread *readThread, *uiThread;
        QCoreApplication *app;
};

#endif //VLBI_SERVER_CGI_H
