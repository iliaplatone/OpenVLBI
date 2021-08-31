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
    char error[150];
    json_settings settings;
    json_value *value = json_parse_ex (&settings,
                                       str, len-1,
                                       error);

    if(!value) {
        fprintf(stderr, "error parsing JSON: %s\n", error);
        return;
    }
    for(int x = 0; x < value->u.object.length; x++) {
        n = value->u.object.values[x].name;
        v = value->u.object.values[x].value;
        if(!strcmp(n, "context")) {
            AddContext(v->u.string.ptr);
            SetContext(v->u.string.ptr);
        }
        if(!strcmp(n, "location")) {
            if(v->u.object.length == 3) {
                double lat = 0, lon = 0, el = 0;
                int i = 0;
                for(int y = 0; y < 3; y ++) {
                    if(!strcmp(v->u.object.values[y].name, "latitude")) {
                        lat = atof(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "longitude")) {
                        lon = atof(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "elevation")) {
                        el = atof(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                }
                if(i == 3) {
                    relative = true;
                    vlbi_set_location(GetContext(), lat, lon, el);
                }
            }
        }
        int i = 0;
        if(!strcmp(n, "fits")) {
            if(v->u.object.length == 2) {
                char *name = nullptr;
                char *buf = nullptr;
                int len;
                char *base64 = nullptr;
                int base64len;
                for(int y = 0; y < 2; y ++) {
                    if(!strcmp(v->u.object.values[y].name, "name")) {
                        name = v->u.object.values[y].value->u.string.ptr;
                    }
                    if(!strcmp(v->u.object.values[y].name, "buffer")) {
                        base64 = v->u.object.values[y].value->u.string.ptr;
                        base64len = v->u.object.values[y].value->u.string.length;
                        buf = (char*)malloc(base64len * 3 / 4);
                        len = from64tobits_fast(buf, base64, base64len);
                        vlbi_add_node(GetContext(), vlbi_file_read_fits(buf, len), name, true);
                        free(buf);
                    }
                }
            }
        }
        if(!strcmp(n, "node")) {
            if(v->u.object.length == 5) {
                timespec_t starttime;
                char *name = nullptr;
                char *buf = nullptr;
                char *locations = nullptr;
                int buflen = 0;
                int locationslen = 0;
                for(int y = 0; y < 5; y ++) {
                    if(!strcmp(v->u.object.values[y].name, "name")) {
                        name = v->u.object.values[y].value->u.string.ptr;
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "starttime")) {
                        starttime = vlbi_time_string_to_utc(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "bitspersample")) {
                        Bps = atoi(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "buffer")) {
                        char *base64 = v->u.object.values[y].value->u.string.ptr;
                        int base64len = v->u.object.values[y].value->u.string.length;
                        buf = (char*)malloc(base64len * 3 / 4);
                        buflen = from64tobits_fast(buf, base64, base64len);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "locations")) {
                        char *base64 = v->u.object.values[y].value->u.string.ptr;
                        int base64len = v->u.object.values[y].value->u.string.length;
                        locations = (char*)malloc(base64len * 3 / 4);
                        locationslen = from64tobits_fast(locations, base64, base64len);
                        i++;
                    }
                }
                if(name && buf && locations && Bps > 0 && starttime.tv_sec > 0) {
                    AddNode(name, (dsp_location*)locations, buf, buflen, starttime, relative);
                }
            }
        }
        if(!strcmp(n, "plot")) {
            if(v->u.object.length == 8) {
                int type = 0;
                for(int y = 0; y < 8; y ++) {
                    if(!strcmp(v->u.object.values[y].name, "target")) {
                        for(int z = 0; z < v->u.object.values[y].value->u.object.length; z ++) {
                            if(!strcmp(v->u.object.values[y].value->u.object.values[z].name, "ra")) {
                                Ra = atof(v->u.object.values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                            if(!strcmp(v->u.object.values[y].value->u.object.values[z].name, "dec")) {
                                Dec = atof(v->u.object.values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                        }
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "frequency")) {
                        Freq = atof(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "samplerate")) {
                        SampleRate = atof(v->u.object.values[y].value->u.string.ptr);
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "resolution")) {
                        for(int z = 0; z < 2; z ++) {
                            if(!strcmp(v->u.object.values[y].value->u.object.values[z].name, "width")) {
                                w = atoi(v->u.object.values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                            if(!strcmp(v->u.object.values[y].value->u.object.values[z].name, "height")) {
                                h = atoi(v->u.object.values[y].value->u.object.values[z].value->u.string.ptr);
                            }
                        }
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "projection")) {
                        if(!strcmp(v->u.object.values[y].value->u.string.ptr, "synthesis"))
                            type |= APERTURE_SYNTHESIS;
                        if(!strcmp(v->u.object.values[y].value->u.string.ptr, "movingbase"))
                            type &= APERTURE_SYNTHESIS;
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "type")) {
                        if(!strcmp(v->u.object.values[y].name, "coverage"))
                            type |= UV_COVERAGE;
                        if(!strcmp(v->u.object.values[y].name, "raw"))
                            type &= UV_COVERAGE;
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "idft")) {
                        type &=  ~UV_IDFT;
                        if(!strcmp(v->u.object.values[y].value->u.string.ptr, "true"))
                            type |=  UV_IDFT;
                        if(!strcmp(v->u.object.values[y].value->u.string.ptr, "1"))
                            type |= UV_IDFT;
                        i++;
                    }
                    if(!strcmp(v->u.object.values[y].name, "adjust_delays")) {
                        nodelay = true;
                        if(!strcmp(v->u.object.values[y].value->u.string.ptr, "true"))
                            nodelay = false;
                        if(!strcmp(v->u.object.values[y].value->u.string.ptr, "1"))
                            nodelay = false;
                        i++;
                    }
                }
                if(i == 8) {
                    dsp_stream_p plot = GetPlot(w, h, type, nodelay);
                    if(type & UV_IDFT) {
                        dsp_stream_p idft = vlbi_get_ifft_estimate(plot);
                        dsp_stream_free_buffer(plot);
                        dsp_stream_free(plot);
                        plot = idft;
                    }
                    dsp_buffer_stretch(plot->buf, plot->len, 0.0, 255.0);
                    int ilen = plot->len;
                    int olen = ilen*4/3+4;
                    unsigned char* buf = (unsigned char*)malloc(plot->len);
                    dsp_buffer_copy(plot->buf, buf, plot->len);
                    unsigned char *base64 = (unsigned char *)malloc(sizeof(dsp_t)*plot->len * 4/3+4);
                    to64frombits(base64, buf, plot->len);
                    fprintf(output, "{\n \"context\": \"%s\",\n \"plot\": {\n  \"projection\": \"%s\",\n  \"type\": \"%s\",\n  \"idft\": %s,\n  \"adjust_delays\": %s,\n  \"resolution\":  {\n   \"width\": \"%d\",\n   \"height\": \"%d\"\n  },\n  \"buffer\": \"%s\"\n }\n}\n", CurrentContext(), (type & APERTURE_SYNTHESIS) ? "synthesis" : "movingbase", (type & UV_COVERAGE) ? "coverage" : "raw", (type & UV_IDFT) ? "true" : "false", nodelay ? "false" : "true", w, h, base64);
                    free(base64);
                    free(buf);
                }
            }
        }
    }
    json_value_free(value);
}

JSONClient *client = new JSONClient();
