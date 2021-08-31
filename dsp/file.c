/*
 *   libDSPAU - a digital signal processing library for astronomy usage
 *   Copyright (C) 2017  Ilia Platone <info@iliaplatone.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsp.h"
#include "vlbi.h"
#include <fitsio.h>

dsp_stream_p vlbi_file_read_fits(void *data, size_t len)
{
    fitsfile *fptr = (fitsfile*)malloc(sizeof(fitsfile));
    memset(fptr, 0, sizeof(fitsfile));
    int bpp = 16;
    int status = 0;
    char key[150];
    char value[150];
    char comment[150];
    char error_status[64];
    dsp_stream_p stream = dsp_stream_new();
    int dim, nelements = 1;
    int anynul = 0;
    void *array;
    int dims;
    long naxes[3] = { 1, 1, 1 };

    fits_open_memfile(&fptr, "", READONLY, &data, &len, 2880, realloc, &status);

    if (status)
    {
        goto fail;
    }

    fits_movabs_hdu(fptr, 1, IMAGE_HDU, &status);
    if(status)
    {
        goto fail;
    }

    fits_get_img_param(fptr, 3, &bpp, &dims, naxes, &status);
    if(status)
    {
        goto fail;
    }

    for(dim = 0; dim < dims; dim++) {
        dsp_stream_add_dim(stream, naxes[dim]);
    }
    dsp_stream_alloc_buffer(stream, stream->len);
    nelements = stream->len;
    array = malloc(abs(bpp) * nelements / 8);
    switch(bpp) {
    case BYTE_IMG:
        fits_read_img(fptr, TBYTE, 1, (long)nelements, NULL, array, &anynul, &status);
        dsp_buffer_stretch(((unsigned char*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((unsigned char*)array), stream->buf, nelements);
        break;
    case SHORT_IMG:
        fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((short*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((short*)array), stream->buf, nelements);
        break;
    case USHORT_IMG:
        fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((unsigned short*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((unsigned short*)array), stream->buf, nelements);
        break;
    case LONG_IMG:
        fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((int*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((int*)array), stream->buf, nelements);
        break;
    case ULONG_IMG:
        fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((unsigned int*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((unsigned int*)array), stream->buf, nelements);
        break;
    case LONGLONG_IMG:
        fits_read_img(fptr, TLONGLONG, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((long*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((long*)array), stream->buf, nelements);
        break;
    case FLOAT_IMG:
        fits_read_img(fptr, TFLOAT, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((float*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((float*)array), stream->buf, nelements);
        break;
    case DOUBLE_IMG:
        fits_read_img(fptr, TDOUBLE, 1, (long)nelements, NULL, array, &anynul, &status);
        if(abs(bpp) > 8*sizeof(dsp_t))
            dsp_buffer_stretch(((double*)(array)), (long)nelements, 0, dsp_t_max);
        dsp_buffer_copy(((double*)array), stream->buf, nelements);
        break;
    }
    free(array);
    if(status||anynul)
        goto fail;

    ffgkey(fptr, "EPOCH", value, comment, &status);
    if (!status)
    {
        double timestamp = atof(value);
        stream->starttimeutc.tv_sec = (time_t)timestamp;
        timestamp -= stream->starttimeutc.tv_sec;
        stream->starttimeutc.tv_nsec = timestamp *  1000000000;
    }
    status = 0;

    ffgkey(fptr, "RA_OBJ", value, comment, &status);
    if (!status)
    {
        stream->target[0] = atof(value);
    }
    status = 0;

    ffgkey(fptr, "DEC_OBJ", value, comment, &status);
    if (!status)
    {
        stream->target[0] = atof(value);
    }
    status = 0;

    ffgkey(fptr, "LATITUDE", value, comment, &status);
    if (!status)
    {
        stream->location->geographic.lat = atof(value);
    }
    status = 0;

    ffgkey(fptr, "LONGITUDE", value, comment, &status);
    if (!status)
    {
        stream->location->geographic.lon = atof(value);
    }
    status = 0;

    ffgkey(fptr, "ELEVATION", value, comment, &status);
    if (!status)
    {
        stream->location->geographic.el = atof(value);
    }
    status = 0;

    ffgkey(fptr, "SRATE", value, comment, &status);
    if (!status)
    {
        stream->samplerate = atof(value);
    }
    status = 0;

    ffgkey(fptr, "FREQ", value, comment, &status);
    if (!status)
    {
        stream->wavelength = LIGHTSPEED/atof(value);
    }
    status = 0;

    status = 0;
    fits_close_file(fptr, &status);
    if(status)
        goto fail;
    return stream;
fail:
    if(status) {
        fits_report_error(stderr, status); /* print out any error messages */
        fits_get_errstatus(status, error_status);
        fprintf(stderr, "FITS Error: %s\n", error_status);
    }
    return NULL;
}
