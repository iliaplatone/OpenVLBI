/*  OpenVLBI - Open Source Very Long Baseline Interferometry
*   Copyright © 2017-2021  Ilia Platone
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

#include "fits_extensions.h"

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

void vlbi_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status)
{
    fits_update_key(fptr, type, name, p, explanation, status);
}

long vlbi_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows)
{
    int status = 0;
    long nrows = 0;
    fits_get_num_rows(fptr, &nrows, &status);
    fits_insert_rows(fptr, nrows, num_rows, &status);
    return nrows;
}


int vlbi_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, unsigned long num_rows)
{
    int status = 0;
    int typecode = 0;
    long repeat = 0;
    long width = 0;

    int ncol = 0;
    long nrow = vlbi_alloc_fits_rows(fptr, num_rows);
    fits_get_colnum(fptr, CASESEN, (char*)(name), &ncol, &status);
    if(status != COL_NOT_FOUND)
    {
        fits_get_eqcoltype(fptr, ncol, &typecode, &repeat, &width, &status);
        fits_write_col(fptr, typecode, ncol, nrow, 1, num_rows, buf, &status);
    }
    return status;
}

int vlbi_append_fits_col(fitsfile *fptr, char* name, char* format)
{
    int status = 0;
    int ncols = 0;
    fits_get_colnum(fptr, CASESEN, (char*)(name), &ncols, &status);
    if(status == COL_NOT_FOUND)
    {
        fits_get_num_cols(fptr, &ncols, &status);
        fits_insert_col(fptr, ncols++, (char*)(name), (char*)(format), &status);
    }
    return ncols;
}

void vlbi_delete_fits_col(fitsfile *fptr, char* name)
{
    int status = 0;
    int ncol = 0;
    fits_get_colnum(fptr, CASESEN, (char*)(name), &ncol, &status);
    while(status != COL_NOT_FOUND)
        fits_delete_col(fptr, ncol, &status);
}

fitsfile* vlbi_create_fits(size_t *size, void **buf)
{
    fitsfile *fptr = NULL;

    size_t memsize;
    int status    = 0;
    char error_status[64];

    //  Now we have to send fits format data to the client
    memsize = 5760;
    void* memptr  = malloc(memsize);
    if (!memptr)
    {
        perr("Error: failed to allocate memory: %lu", (unsigned long)(memsize));
    }

    fits_create_memfile(&fptr, &memptr, &memsize, 2880, realloc, &status);

    if (status)
    {
        fits_report_error(stderr, status); /* print out any error messages */
        fits_get_errstatus(status, error_status);
        perr("FITS Error: %s", error_status);
        if(memptr != NULL)
            free(memptr);
        return NULL;
    }
    if (status)
    {
        fits_report_error(stderr, status); /* print out any error messages */
        fits_get_errstatus(status, error_status);
        perr("FITS Error: %s", error_status);
        if(memptr != NULL)
            free(memptr);
        return NULL;
    }

    if (status)
    {
        fits_report_error(stderr, status); /* print out any error messages */
        fits_get_errstatus(status, error_status);
        perr("FITS Error: %s", error_status);
        if(memptr != NULL)
            free(memptr);
        return NULL;
    }

    *size = memsize;
    *buf = memptr;

    return fptr;
}

int vlbi_close_fits(fitsfile *fptr)
{
    int status = 0;
    fits_close_file(fptr, &status);

    return status;
}

dsp_stream_p vlbi_read_fits(char *filename)
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
    int anynul = 0;
    void *array;
    int dims;
    long naxes[3] = { 1, 1, 1 };

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
        dsp_stream_add_dim(stream, naxes[dim]);
    }
    dsp_stream_alloc_buffer(stream, stream->len);
    nelements = stream->len;
    array = malloc(abs(bpp) * nelements / 8);
    switch(bpp)
    {
        case BYTE_IMG:
            fits_read_img(fptr, TBYTE, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_stretch(((unsigned char*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned char*)array), stream->buf, nelements);
            break;
        case SHORT_IMG:
            fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((short*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((short*)array), stream->buf, nelements);
            break;
        case USHORT_IMG:
            fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((unsigned short*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned short*)array), stream->buf, nelements);
            break;
        case LONG_IMG:
            fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((int*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((int*)array), stream->buf, nelements);
            break;
        case ULONG_IMG:
            fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((unsigned int*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned int*)array), stream->buf, nelements);
            break;
        case LONGLONG_IMG:
            fits_read_img(fptr, TLONGLONG, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((long*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((long*)array), stream->buf, nelements);
            break;
        case FLOAT_IMG:
            fits_read_img(fptr, TFLOAT, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((float*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((float*)array), stream->buf, nelements);
            break;
        case DOUBLE_IMG:
            fits_read_img(fptr, TDOUBLE, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((double*)(array)), (long)nelements, 0, dsp_t_max);
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

dsp_stream_p vlbi_read_sdfits(char *filename)
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
    int anynul = 0;
    void *array;
    int dims;
    long naxes[3] = { 1, 1, 1 };

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
        dsp_stream_add_dim(stream, naxes[dim]);
    }
    dsp_stream_alloc_buffer(stream, stream->len);
    nelements = stream->len;
    array = malloc(abs(bpp) * nelements / 8);
    switch(bpp)
    {
        case BYTE_IMG:
            fits_read_img(fptr, TBYTE, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_stretch(((unsigned char*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned char*)array), stream->buf, nelements);
            break;
        case SHORT_IMG:
            fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((short*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((short*)array), stream->buf, nelements);
            break;
        case USHORT_IMG:
            fits_read_img(fptr, TUSHORT, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((unsigned short*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned short*)array), stream->buf, nelements);
            break;
        case LONG_IMG:
            fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((int*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((int*)array), stream->buf, nelements);
            break;
        case ULONG_IMG:
            fits_read_img(fptr, TULONG, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((unsigned int*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned int*)array), stream->buf, nelements);
            break;
        case LONGLONG_IMG:
            fits_read_img(fptr, TLONGLONG, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((long*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((long*)array), stream->buf, nelements);
            break;
        case FLOAT_IMG:
            fits_read_img(fptr, TFLOAT, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((float*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((float*)array), stream->buf, nelements);
            break;
        case DOUBLE_IMG:
            fits_read_img(fptr, TDOUBLE, 1, (long)nelements, NULL, array, &anynul, &status);
            if(abs(bpp) > 8 * sizeof(dsp_t))
                dsp_buffer_stretch(((double*)(array)), (long)nelements, 0, dsp_t_max);
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
