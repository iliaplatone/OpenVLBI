/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2022  Ilia Platone
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 3 of the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <vlbi.h>

static int f_scansexa(const char *str0, /* input string */
                      double *dp)       /* cracked value, if return 0 */
{
    double a = 0, b = 0, c = 0;
    char str[128];
    //char *neg;
    unsigned char isNegative = 0;
    int r = 0;

    /* copy str0 so we can play with it */
    strncpy(str, str0, sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';

    /* remove any spaces */
    char* i = str;
    char* j = str;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
            i++;
    }
    *i = 0;

    // This has problem process numbers in scientific notations e.g. 1e-06
    /*neg = strchr(str, '-');
    if (neg)
        *neg = ' ';
    */
    if (str[0] == '-')
    {
        isNegative = 1;
        str[0] = ' ';
    }

    r = sscanf(str, "%lf%*[^0-9]%lf%*[^0-9]%lf", &a, &b, &c);


    if (r < 1)
        return (-1);
    *dp = a + b / 60 + c / 3600;
    if (isNegative)
        *dp *= -1;
    return (0);
}

dsp_stream_p * vlbi_file_read_sdfits(char * filename, long *n)
{
    long nrows = 0;
    long dim;
    long dims, *sizes;
    int typecode = 0;
    long width = 0;
    long repeat = 0;
    int r = 0;
    dsp_fits_row *rows = dsp_fits_read_sdfits(filename, &nrows, &dims, &sizes);
    dsp_stream_p *stream = (dsp_stream_p*)malloc(sizeof(dsp_stream_p) * (size_t)nrows);
    for(r = 0; r < nrows; r++)
    {
        stream[r] = dsp_stream_new();
        int k = 0;
        dsp_fits_row row = rows[r];
        for(k = 0; strcmp(SDFITS_COLUMN_OBJCTRA.name, row.columns[k].name); k++);
        f_scansexa(row.columns[k].value, &stream[r]->target[0]);
        for(k = 0; strcmp(SDFITS_COLUMN_OBJCTDEC.name, row.columns[k].name); k++);
        f_scansexa(row.columns[k].value, &stream[r]->target[1]);
        for(k = 0; strcmp(SDFITS_COLUMN_OBJCTDEC.name, row.columns[k].name); k++);
        f_scansexa(row.columns[k].value, &stream[r]->target[1]);
        for(k = 0; strcmp(SDFITS_COLUMN_OBSFREQ.name, row.columns[k].name); k++);
        stream[r]->wavelength = *((double*)row.columns[k].value);
        stream[r]->wavelength = SPEED_MEAN / stream[r]->wavelength;
        for(k = 0; strcmp(SDFITS_COLUMN_SITELAT.name, row.columns[k].name); k++);
        stream[r]->location[0].geographic.lat = *((double*)row.columns[k].value);
        for(k = 0; strcmp(SDFITS_COLUMN_SITELONG.name, row.columns[k].name); k++);
        stream[r]->location[0].geographic.lon = *((double*)row.columns[k].value);
        for(k = 0; strcmp(SDFITS_COLUMN_SITEELEV.name, row.columns[k].name); k++);
        stream[r]->location[0].geographic.el = *((double*)row.columns[k].value);
        for(k = 0; strcmp(SDFITS_COLUMN_DATE_OBS.name, row.columns[k].name); k++);
        stream[r]->starttimeutc = vlbi_time_string_to_timespec(row.columns[k].value);
        for(k = 0; strcmp(SDFITS_COLUMN_EXPOSURE.name, row.columns[k].name); k++);
        stream[r]->samplerate = *((double*)row.columns[k].value);
        for(k = 0; strcmp(SDFITS_COLUMN_TIME.name, row.columns[k].name); k++);
        double time = (long) * ((double*)row.columns[k].value);
        time -= stream[r]->samplerate / 2.0;
        stream[r]->starttimeutc.tv_sec += (long)time;
        stream[r]->starttimeutc.tv_nsec += (long)((time - stream[r]->starttimeutc.tv_sec) * 1000000000);

        for(k = 0; strcmp(SDFITS_COLUMN_DATA.name, row.columns[k].name); k++);

        dsp_fits_read_typecode(row.columns[k].format, &typecode, &width, &repeat);
        dsp_stream_add_dim(stream[r], (int)width);
        dsp_stream_add_dim(stream[r], (int)repeat);
        for(dim = 0; dim < dims; dim++)
        {
            dsp_stream_add_dim(stream[r], (int)sizes[dim]);
        }
        dsp_stream_alloc_buffer(stream[r], stream[r]->len);

        stream[r]->samplerate /= stream[r]->len;
        stream[r]->samplerate = 1.0 / stream[r]->samplerate;
        switch(typecode)
        {
            case TBYTE:
            case TSBYTE:
            case TLOGICAL:
                dsp_buffer_copy(((unsigned char*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TSHORT:
                dsp_buffer_copy(((short*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TUSHORT:
                dsp_buffer_copy(((unsigned short*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TINT:
                dsp_buffer_copy(((int*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TUINT:
                dsp_buffer_copy(((unsigned int*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TLONG:
                dsp_buffer_copy(((long*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TULONG:
                dsp_buffer_copy(((unsigned long*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TFLOAT:
                dsp_buffer_copy(((float*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TDOUBLE:
                dsp_buffer_copy(((double*)row.columns[k].value), stream[r]->buf, stream[r]->len);
                break;
            case TCOMPLEX:
                dsp_buffer_copy(((float*)row.columns[k].value), stream[r]->dft.buf, stream[r]->len * 2);
                break;
            case TDBLCOMPLEX:
                dsp_buffer_copy(((double*)row.columns[k].value), stream[r]->dft.buf, stream[r]->len * 2);
                break;
            default:
                break;
        }
    }
    *n = nrows;
    return stream;
}

dsp_stream_p vlbi_file_read_fits(char *filename)
{
    fitsfile *fptr = (fitsfile*)malloc(sizeof(fitsfile));
    memset(fptr, 0, sizeof(fitsfile));
    int bpp = 16;
    int status = 0;
    char value[150];
    char comment[150];
    char error_status[64];
    dsp_stream_p stream = dsp_stream_new();
    int dim, nelements = 1;
    int dims;
    long naxes[3] = { 1, 1, 1 };
    int anynul = 0;
    void *array = NULL;

    fits_open_file(&fptr, filename, READONLY, &status);

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

    for(dim = 0; dim < dims; dim++)
    {
        dsp_stream_add_dim(stream, (int)naxes[dim]);
    }
    dsp_stream_alloc_buffer(stream, stream->len);
    nelements = stream->len;

    array = malloc((size_t)(abs(bpp) * nelements / 8));
    switch(bpp)
    {
        case BYTE_IMG:
            fits_read_img(fptr, TBYTE, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((unsigned char*)array), stream->buf, nelements);
            break;
        case SHORT_IMG:
            fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((short*)array), stream->buf, nelements);
            break;
        case USHORT_IMG:
            fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((unsigned short*)array), stream->buf, nelements);
            break;
        case LONG_IMG:
            fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((int*)array), stream->buf, nelements);
            break;
        case ULONG_IMG:
            fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((unsigned int*)array), stream->buf, nelements);
            break;
        case LONGLONG_IMG:
            fits_read_img(fptr, TLONGLONG, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((long*)array), stream->buf, nelements);
            break;
        case FLOAT_IMG:
            fits_read_img(fptr, TFLOAT, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((float*)array), stream->buf, nelements);
            break;
        case DOUBLE_IMG:
            fits_read_img(fptr, TDOUBLE, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((double*)array), stream->buf, nelements);
            break;
    }
    free(array);
    if(status || anynul)
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

    ffgkey(fptr, "OBJCTRA", value, comment, &status);
    if (!status)
    {
        f_scansexa(value, &stream->target[0]);
    }
    status = 0;

    ffgkey(fptr, "OBJCTDEC", value, comment, &status);
    if (!status)
    {
        f_scansexa(value, &stream->target[1]);
    }
    status = 0;

    ffgkey(fptr, "SITELAT", value, comment, &status);
    if (!status)
    {
        stream->location->geographic.lat = atof(value);
    }
    status = 0;

    ffgkey(fptr, "SITELONG", value, comment, &status);
    if (!status)
    {
        stream->location->geographic.lon = atof(value);
    }
    status = 0;

    ffgkey(fptr, "SITEELEV", value, comment, &status);
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
        stream->wavelength = SPEED_MEAN / atof(value);
    }
    status = 0;

    status = 0;
    fits_close_file(fptr, &status);
    if(status)
        goto fail;
    return stream;
fail:
    if(status)
    {
        fits_report_error(stderr, status); /* print out any error messages */
        fits_get_errstatus(status, error_status);
        fprintf(stderr, "FITS Error: %s\n", error_status);
    }
    return NULL;
}
