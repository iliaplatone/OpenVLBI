#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vlbi.h>
#include <fitsio2.h>
#include <json.h>
#include <base64.h>
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

void JSONClient::Parse()
{
    FILE* f = input;
    char *str = nullptr;
    json_value *v;
    char *n;
    size_t len = 0;
    getdelim(&str, &len, (int)'\0', f);
    if(strlen(str) <= 0)
        return;
    char error[json_error_max];
    json_settings settings;
    json_value *value = json_parse_ex (&settings,
                                       str, len - 2,
                                       error);
    if(!value)
    {
        fprintf(stderr, "error parsing JSON: %s\n", error);
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
            AddContext(v->u.string.ptr);
            SetContext(v->u.string.ptr);
        }
        if(!strcmp(n, "location"))
        {
            if(v->u.object.length == 3)
            {
                double lat = 0, lon = 0, el = 0;
                int i = 0;
                for(int y = 0; y < 3; y ++)
                {
                    if(!strcmp(values[y].name, "latitude"))
                    {
                        lat = atof(values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(values[y].name, "longitude"))
                    {
                        lon = atof(values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(values[y].name, "elevation"))
                    {
                        el = atof(values[y].value->u.string.ptr);
                        i++;
                    }
                }
                if(i == 3)
                {
                    relative = true;
                    vlbi_set_location(GetContext(), lat, lon, el);
                }
            }
        }
        int i = 0;
        if(!strcmp(n, "fits"))
        {
            if(v->u.object.length == 2)
            {
                char *name = nullptr;
                char *buf = nullptr;
                int len;
                char *base64 = nullptr;
                int base64len;
                for(int y = 0; y < 2; y ++)
                {
                    if(!strcmp(values[y].name, "name"))
                    {
                        name = values[y].value->u.string.ptr;
                    }
                    if(!strcmp(values[y].name, "buffer"))
                    {
                        base64 = values[y].value->u.string.ptr;
                        base64len = values[y].value->u.string.length;
                        buf = (char*)malloc(base64len * 3 / 4);
                        len = from64tobits_fast(buf, base64, base64len);
                        vlbi_add_node(GetContext(), vlbi_file_read_fits(buf, len), name, true);
                        free(buf);
                    }
                }
            }
        }
        if(!strcmp(n, "node"))
        {
            if(v->u.object.length == 5)
            {
                timespec_t starttime;
                char *name = nullptr;
                char *buf = nullptr;
                char *locations = nullptr;
                int buflen = 0;
                int locationslen = 0;
                for(int y = 0; y < 5; y ++)
                {
                    if(!strcmp(values[y].name, "name"))
                    {
                        name = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "starttime"))
                    {
                        starttime = vlbi_time_string_to_timespec(values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(values[y].name, "bitspersample"))
                    {
                        Bps = atoi(values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(values[y].name, "buffer"))
                    {
                        char *base64 = values[y].value->u.string.ptr;
                        int base64len = values[y].value->u.string.length;
                        buf = (char*)malloc(base64len * 3 / 4);
                        buflen = from64tobits_fast(buf, base64, base64len);
                        i++;
                    }
                    if(!strcmp(values[y].name, "locations"))
                    {
                        char *base64 = values[y].value->u.string.ptr;
                        int base64len = values[y].value->u.string.length;
                        locations = (char*)malloc(base64len * 3 / 4);
                        locationslen = from64tobits_fast(locations, base64, base64len);
                        i++;
                    }
                }
                if(name && buf && locations && Bps > 0 && starttime.tv_sec > 0)
                {
                    AddNode(name, (dsp_location*)locations, buf, buflen, starttime, relative);
                }
            }
        }
        if(!strcmp(n, "plot"))
        {
            if(v->u.object.length == 8)
            {
                int type = 0;
                char *name = nullptr;
                for(int y = 0; y < 8; y ++)
                {
                    if(!strcmp(values[y].name, "name"))
                    {
                        name = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "target"))
                    {
                        for(unsigned int z = 0; z < values[y].value->u.object.length; z ++)
                        {
                            if(!strcmp(values[y].value->u.object.values[z].name, "ra"))
                            {
                                Ra = atof(values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                            if(!strcmp(values[y].value->u.object.values[z].name, "dec"))
                            {
                                Dec = atof(values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                        }
                        i++;
                    }
                    if(!strcmp(values[y].name, "frequency"))
                    {
                        Freq = atof(values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(values[y].name, "samplerate"))
                    {
                        SampleRate = atof(values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(values[y].name, "resolution"))
                    {
                        for(int z = 0; z < 2; z ++)
                        {
                            if(!strcmp(values[y].value->u.object.values[z].name, "width"))
                            {
                                w = atoi(values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                            if(!strcmp(values[y].value->u.object.values[z].name, "height"))
                            {
                                h = atoi(values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                        }
                        i++;
                    }
                    if(!strcmp(values[y].name, "projection"))
                    {
                        if(!strcmp(values[y].value->u.string.ptr, "synthesis"))
                            type |= APERTURE_SYNTHESIS;
                        if(!strcmp(values[y].value->u.string.ptr, "movingbase"))
                            type &= APERTURE_SYNTHESIS;
                        i++;
                    }
                    if(!strcmp(values[y].name, "type"))
                    {
                        if(!strcmp(values[y].value->u.string.ptr, "coverage"))
                            type |= UV_COVERAGE;
                        if(!strcmp(values[y].value->u.string.ptr, "raw"))
                            type &= ~UV_COVERAGE;
                        i++;
                    }
                    if(!strcmp(values[y].name, "adjust_delays"))
                    {
                        nodelay = true;
                        if(!strcmp(values[y].value->u.string.ptr, "true"))
                            nodelay = false;
                        if(!strcmp(values[y].value->u.string.ptr, "1"))
                            nodelay = false;
                        i++;
                    }
                }
                if(i == 8)
                {
                    Plot(name, w, h, type, nodelay);
                }
            }
        }
        if(!strcmp(n, "download"))
        {
            if(v->u.object.length == 2)
            {
                char* name = nullptr;
                char* format = nullptr;
                for(int y = 0; y < 2; y ++)
                {
                    if(!strcmp(values[y].name, "name"))
                    {
                        name = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "format"))
                    {
                        format = values[y].value->u.string.ptr;
                        i++;
                    }
                }
                if(i == 2)
                {
                    char *base64 = GetModel(name, format);
                    fprintf(output,
                            "{\n \"context\": \"%s\",\n \"model\": {\n  \"name\": \"%s\",\n  \"format\": \"%s\",\n  \"buffer\": \"%s\"\n }\n}\n",
                            CurrentContext(), name, format, base64);
                    free(base64);
                }
            }
        }
        if(!strcmp(n, "upload"))
        {
            if(v->u.object.length == 3)
            {
                char* name = nullptr;
                char* format = nullptr;
                char* base64 = nullptr;
                for(int y = 0; y < 3; y ++)
                {
                    if(!strcmp(values[y].name, "name"))
                    {
                        name = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "format"))
                    {
                        format = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "buffer"))
                    {
                        base64 = values[y].value->u.string.ptr;
                        i++;
                    }
                }
                if(i == 2)
                {
                    AddModel(name, format, base64);
                }
            }
        }
        if(!strcmp(n, "dft"))
        {
            if(v->u.object.length == 4)
            {
                bool inverse = false;
                char* idft = nullptr;
                char* magnitude = nullptr;
                char* phase = nullptr;
                for(int y = 0; y < 4; y ++)
                {
                    if(!strcmp(values[y].name, "inverse"))
                    {
                        inverse = !strcmp(values[y].value->u.string.ptr, "true") ? true : false;
                        i++;
                    }
                    if(!strcmp(values[y].name, "idft"))
                    {
                        idft = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "magnitude"))
                    {
                        magnitude = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "phase"))
                    {
                        phase = values[y].value->u.string.ptr;
                        i++;
                    }
                }
                if(i == 4)
                {
                    if(inverse)
                    {
                        SetMagnitude(idft, magnitude);
                        SetPhase(idft, phase);
                        Idft(idft);
                    }
                    else
                    {
                        Dft(idft, magnitude, phase);
                    }
                }
            }
        }
        if(!strcmp(n, "edit"))
        {
            if(v->u.object.length == 4)
            {
                char* name = nullptr;
                char* model = nullptr;
                char* edit = nullptr;
                char* arg = nullptr;
                for(int y = 0; y < 4; y ++)
                {
                    if(!strcmp(values[y].name, "name"))
                    {
                        name = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "model"))
                    {
                        name = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "arg"))
                    {
                        arg = values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(values[y].name, "edit"))
                    {
                        edit = values[y].value->u.string.ptr;
                        i++;
                    }
                }
                if(i == 4)
                {
                    if(edit != nullptr)
                    {
                        if(!strcmp(edit, "shift"))
                        {
                            if(name != nullptr)
                                Shift(name);
                        }
                        if(!strcmp(edit, "mask"))
                        {
                            if(name != nullptr && model != nullptr && arg != nullptr)
                                Mask(name, model, arg);
                        }
                    }
                }
            }
        }
    }
    json_value_free(value);
}

JSONClient *client = new JSONClient();
