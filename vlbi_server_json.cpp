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
#include <base64.h>
#include "strings.h"
#include "mjs.h"
#include "json.h"
#include "vlbi_server_json.h"

static char *js_code_str;

static double js_callback(double a, double b) {
    struct mjs *mjs = mjs_create();
    char *str = (char*)malloc(strlen(js_code_str) + 100);
    sprintf(str, "function callback(a, b) { %s }; callback(%lf, %lf);", js_code_str, a, b);
    mjs_val_t result;
    mjs_exec(mjs, str, &result);
    return (double)result;
}

JSONServer::JSONServer()
{
    js_code_str = (char*)malloc(1);
}

JSONServer::~JSONServer()
{
    free(js_code_str);
}

int JSONServer::Init(int argc, char** argv)
{
    return VLBI::Server::Init(argc, argv);
}

bool JSONServer::CheckMask(unsigned long mask, int n)
{
    return ((mask & ((1<<n)-1)) ^ ((1<<n)-1)) == 0;
}

void JSONServer::Parse()
{
    FILE* f = getInput();
    json_value *v;
    char *n;
    size_t len = 0;
    char *str = (char*)malloc(1);
    while(true) {
        str[len] = 0;
        fread(&str[len], 1, 1, f);
        if(str[len++] == 0) break;
        str = (char*)realloc(str, len + 8);
    }
    if(strlen(str) <= 0)
        return;
    if (len == 0)
        return;
    char error[json_error_max];
    json_settings settings;
    memset(&settings, 0, sizeof(json_settings));
    json_value *value = json_parse_ex (&settings,
                                       str, len,
                                       error);
    if(!value)
    {
        perr("error parsing JSON: %s\n", error);
        return;
    }
    for(unsigned int x = 0; x < value->u.object.length; x++)
    {
        n = value->u.object.values[x].name;
        v = value->u.object.values[x].value;
        typedef struct
        {
            json_char * name;
            unsigned int name_length;

            struct _json_value * value;

        }* json_values;
        json_values values = (json_values)v->u.object.values;
        if(!strcmp(n, "context"))
        {
            addContext(v->u.string.ptr);
            setContext(v->u.string.ptr);
        }
        if(!strcmp(n, "location"))
        {
            unsigned long mask = 0;
            double lat = 0, lon = 0, el = 0;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "latitude"))
                {
                    lat = atof(values[y].value->u.string.ptr);
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "longitude"))
                {
                    lon = atof(values[y].value->u.string.ptr);
                    mask |= 1 << 1;
                }
                if(!strcmp(values[y].name, "elevation"))
                {
                    el = atof(values[y].value->u.string.ptr);
                    mask |= 1 << 2;
                }
            }
            if(CheckMask(mask, 3))
            {
                relative = true;
                vlbi_set_location(getContext(), lat, lon, el);
            }
        }
        if(!strcmp(n, "fits"))
        {
            unsigned long mask = 0;
            char *name = nullptr;
            char *base64 = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "buffer"))
                {
                    base64 = values[y].value->u.string.ptr;
                    mask |= 1 << 1;
                }
            }
            if(CheckMask(mask, 2))
            {
                addNode(name, base64);
            }
        }
        if(!strcmp(n, "sdfits"))
        {
            unsigned long mask = 0;
            char *name = nullptr;
            char *base64 = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "buffer"))
                {
                    base64 = values[y].value->u.string.ptr;
                    mask |= 1 << 1;
                }
            }
            if(CheckMask(mask, 2))
            {
                addNodes(name, base64);
            }
        }
        if(!strcmp(n, "node"))
        {
            unsigned long mask = 0;
            timespec_t starttime;
            char *name = nullptr;
            char *buf = nullptr;
            char *locations = nullptr;
            int buflen = 0;
            int locationslen = 0;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "starttime"))
                {
                    starttime = vlbi_time_string_to_timespec(values[y].value->u.string.ptr);
                    mask |= 1 << 1;
                }
                if(!strcmp(values[y].name, "bitspersample"))
                {
                    setBps(atoi(values[y].value->u.string.ptr));
                    mask |= 1 << 2;
                }
                if(!strcmp(values[y].name, "buffer"))
                {
                    char *base64 = values[y].value->u.string.ptr;
                    int base64len = values[y].value->u.string.length;
                    buf = (char*)malloc(base64len * 3 / 4);
                    buflen = from64tobits_fast(buf, base64, base64len);
                    mask |= 1 << 3;
                }
                if(!strcmp(values[y].name, "locations"))
                {
                    char *base64 = values[y].value->u.string.ptr;
                    int base64len = values[y].value->u.string.length;
                    locations = (char*)malloc(base64len * 3 / 4);
                    locationslen = from64tobits_fast(locations, base64, base64len);
                    mask |= 1 << 4;
                }
            }
            if(CheckMask(mask, 5))
            {
                addNode(name, (dsp_location*)locations, buf, buflen, starttime, relative);
            }
        }
        if(!strcmp(n, "plot"))
        {
            unsigned long mask = 0;
            int flags = 0;
            char *name = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "target"))
                {
                    for(unsigned int z = 0; z < values[y].value->u.object.length; z ++)
                    {
                        if(!strcmp(values[y].value->u.object.values[z].name, "ra"))
                        {
                            setRa(atof(values[y].value->u.object.values[z].value->u.string.ptr));
                            mask |= 1 << 1;
                        }
                        if(!strcmp(values[y].value->u.object.values[z].name, "dec"))
                        {
                            setDec(atof(values[y].value->u.object.values[z].value->u.string.ptr));
                            mask |= 1 << 2;
                        }
                    }
                }
                if(!strcmp(values[y].name, "frequency"))
                {
                    setFreq(atof(values[y].value->u.string.ptr));
                    mask |= 1 << 3;
                }
                if(!strcmp(values[y].name, "samplerate"))
                {
                    setSampleRate(atof(values[y].value->u.string.ptr));
                    mask |= 1 << 3;
                }
                if(!strcmp(values[y].name, "resolution"))
                {
                    for(int z = 0; z < 2; z ++)
                    {
                        if(!strcmp(values[y].value->u.object.values[z].name, "width"))
                        {
                            setWidth(atoi(values[y].value->u.object.values[z].value->u.string.ptr));
                            mask |= 1 << 4;
                        }
                        if(!strcmp(values[y].value->u.object.values[z].name, "height"))
                        {
                            setHeight(atoi(values[y].value->u.object.values[z].value->u.string.ptr));
                            mask |= 1 << 5;
                        }
                    }
                }
                if(!strcmp(values[y].name, "projection"))
                {
                    if(!strcmp(values[y].value->u.string.ptr, "synthesis"))
                        flags &= ~plot_flags_moving_baseline;
                    if(!strcmp(values[y].value->u.string.ptr, "movingbase"))
                        flags |= plot_flags_moving_baseline;
                    mask |= 1 << 6;
                }
                if(!strcmp(values[y].name, "type"))
                {
                    if(!strcmp(values[y].value->u.string.ptr, "coverage"))
                        flags |= plot_flags_uv_coverage;
                    else if(!strcmp(values[y].value->u.string.ptr, "default"))
                        flags &= ~plot_flags_uv_coverage;
                    else {
                        for(int z = 0; z < (int)v->u.object.length; z ++)
                        {
                            if(!strcmp(values[z].name, values[y].value->u.string.ptr))
                            {
                                size_t len = strlen(values[z].value->u.string.ptr);
                                js_code_str = (char*)realloc(js_code_str, len);
                                memcpy(js_code_str, values[z].value->u.string.ptr, len);
                                setDelegate(js_callback);
                                flags |= plot_flags_custom_delegate;
                                mask |= 1 << 9;
                            }
                        }
                    }
                    mask |= 1 << 7;
                }
                if(!strcmp(values[y].name, "adjust_delays"))
                {
                    flags |= plot_flags_synced;
                    if(!strcmp(values[y].value->u.string.ptr, "true"))
                        flags &= ~plot_flags_synced;
                    if(!strcmp(values[y].value->u.string.ptr, "1"))
                        flags &= ~plot_flags_synced;
                    mask |= 1 << 8;
                }
            }
            if(CheckMask(mask, 9))
            {
                Plot(name, flags);
            }
        }
        if(!strcmp(n, "download"))
        {
            unsigned long mask = 0;
            char* name = nullptr;
            char* format = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "format"))
                {
                    format = values[y].value->u.string.ptr;
                    mask |= 1 << 1;
                }
            }
            if(CheckMask(mask, 2))
            {
                char *base64 = getModel(name, format);
                fprintf(getOutput(),
                        "{\n \"context\": \"%s\",\n \"model\": {\n  \"name\": \"%s\",\n  \"format\": \"%s\",\n  \"buffer\": \"%s\"\n }\n}\n", currentContext(), name, format, base64);
                free(base64);
            }
        }
        if(!strcmp(n, "upload"))
        {
            unsigned long mask = 0;
            char* name = nullptr;
            char* format = nullptr;
            char* base64 = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "format"))
                {
                    format = values[y].value->u.string.ptr;
                    mask |= 1 << 1;
                }
                if(!strcmp(values[y].name, "buffer"))
                {
                    base64 = values[y].value->u.string.ptr;
                    mask |= 1 << 2;
                }
            }
            if(CheckMask(mask, 3))
            {
                addModel(name, format, base64);
            }
        }
        if(!strcmp(n, "dft"))
        {
            unsigned long mask = 0;
            bool inverse = false;
            char* idft = nullptr;
            char* magnitude = nullptr;
            char* phase = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "inverse"))
                {
                    inverse = strcmp(values[y].value->u.string.ptr, "false") ? true : false;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "idft"))
                {
                    idft = values[y].value->u.string.ptr;
                    mask |= 1 << 1;
                }
                if(!strcmp(values[y].name, "magnitude"))
                {
                    magnitude = values[y].value->u.string.ptr;
                    mask |= 1 << 2;
                }
                if(!strcmp(values[y].name, "phase"))
                {
                    phase = values[y].value->u.string.ptr;
                    mask |= 1 << 3;
                }
            }
            if(CheckMask(mask, 4))
            {
                if(inverse)
                {
                    Idft(idft, magnitude, phase);
                }
                else
                {
                    Dft(idft, magnitude, phase);
                }
            }
        }
        if(!strcmp(n, "edit"))
        {
            unsigned long mask = 0;
            char* name = nullptr;
            char* model = nullptr;
            char* node = nullptr;
            char* edit = nullptr;
            char* arg = nullptr;
            char* arg1 = nullptr;
            char* arg2 = nullptr;
            for(int y = 0; y < (int)v->u.object.length; y ++)
            {
                if(!strcmp(values[y].name, "edit"))
                {
                    edit = values[y].value->u.string.ptr;
                    mask |= 1 << 0;
                }
                if(!strcmp(values[y].name, "name"))
                {
                    name = values[y].value->u.string.ptr;
                    mask |= 1 << 1;
                }
                if(!strcmp(values[y].name, "model"))
                {
                    model = values[y].value->u.string.ptr;
                    mask |= 1 << 2;
                }
                if(!strcmp(values[y].name, "arg"))
                {
                    arg = values[y].value->u.string.ptr;
                    mask |= 1 << 3;
                }
                if(!strcmp(values[y].name, "node"))
                {
                    node = values[y].value->u.string.ptr;
                    mask |= 1 << 4;
                }
                if(!strcmp(values[y].name, "arg1"))
                {
                    arg1 = values[y].value->u.string.ptr;
                    mask |= 1 << 5;
                }
                if(!strcmp(values[y].name, "arg2"))
                {
                    arg2 = values[y].value->u.string.ptr;
                    mask |= 1 << 6;
                }
            }
            if(CheckMask(mask, 1))
            {
                if(CheckMask(mask, 2)) {
                    if(!strcmp(edit, "shift"))
                    {
                        if(strcmp(name, ""))
                            Shift(name);
                    }
                    if(CheckMask(mask, 4)) {
                        if(!strcmp(edit, "mask"))
                        {
                            Mask(name, model, arg);
                        }
                        else if(!strcmp(edit, "stack"))
                        {
                            Stack(name, model, arg);
                        }
                        else if(!strcmp(edit, "diff"))
                        {
                            Diff(name, model, arg);
                        }
                        else if(!strcmp(edit, "convolve"))
                        {
                            Convolve(name, model, arg);
                        }
                    }
                }
            } else {
                mask >>= 5;
                if(CheckMask(mask, 1)) {
                    if(!strcmp(edit, "lowpass"))
                    {
                        if(strcmp(node, ""))
                            LowPass(name, node, atof(arg1));
                    }
                    if(!strcmp(edit, "highpass"))
                    {
                        if(strcmp(name, ""))
                            HighPass(name, node, atof(arg1));
                    }
                    if(CheckMask(mask, 2)) {
                        if(!strcmp(edit, "bandpass"))
                        {
                            if(strcmp(node, ""))
                                BandPass(name, node, atof(arg1), atof(arg2));
                        }
                        if(!strcmp(edit, "bandreject"))
                        {
                            if(strcmp(name, ""))
                                BandReject(name, node, atof(arg1), atof(arg2));
                        }
                    }
                }
            }
        }
    }
    json_value_free(value);
}

Server *VLBI::server = (Server*)new JSONServer();
