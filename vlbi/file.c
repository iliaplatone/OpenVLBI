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

void extfits_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status)
{
    fits_update_key(fptr, type, name, p, explanation, status);
}

long extfits_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows)
{
    int status = 0;
    long nrows = 0;
    fits_get_num_rows(fptr, &nrows, &status);
    fits_insert_rows(fptr, nrows, num_rows, &status);
    return nrows;
}


int extfits_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, unsigned long num_rows)
{
    int status = 0;
    int typecode = 0;
    long repeat = 0;
    long width = 0;

    int ncol = 0;
    long nrow = extfits_alloc_fits_rows(fptr, num_rows);
    fits_get_colnum(fptr, CASESEN, (char*)(name), &ncol, &status);
    if(status != COL_NOT_FOUND)
    {
        fits_get_eqcoltype(fptr, ncol, &typecode, &repeat, &width, &status);
        fits_write_col(fptr, typecode, ncol, nrow, 1, num_rows, buf, &status);
    }
    return status;
}

int extfits_append_fits_col(fitsfile *fptr, char* name, char* format)
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

void extfits_delete_fits_col(fitsfile *fptr, char* name)
{
    int status = 0;
    int ncol = 0;
    fits_get_colnum(fptr, CASESEN, (char*)(name), &ncol, &status);
    while(status != COL_NOT_FOUND)
        fits_delete_col(fptr, ncol, &status);
}

fitsfile* extfits_create_fits(size_t *size, void **buf)
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

int extfits_close_fits(fitsfile *fptr)
{
    int status = 0;
    fits_close_file(fptr, &status);

    return status;
}

int extfits_check_column(fitsfile *fptr, char* column, char **expected, int expected_n, int rown)
{
    int err = 1, n = 0, anynul = 0, status = 0, x, y, typecode;
    long repeat = 1;
    long width;
    char **value = (char **)malloc(sizeof(char*));
    value[0] = (char*) malloc(150);
    if(column == NULL || expected == NULL)
        goto err_return;
    fits_get_colname(fptr, 0, column, value[0], &n, &status);
    if(status)
        goto err_return;
    fits_get_coltype(fptr, n, &typecode, &repeat, &width, &status);
    if(typecode != TSTRING)
        goto err_return;
    free(value[0]);
    value = (char **)realloc(value, sizeof(char*)*(size_t)repeat);
    for(x = 0; x < repeat; x++) {
        value[x] = (char*) malloc((size_t)width);
        fits_read_col_str(fptr, n, rown, 1, 1, NULL, value, &anynul, &status);
        for(y = 0; y < expected_n; y++) {
            if(!strcmp(value[x], expected[y])) {
                err &= 1;
                break;
            }
        }
    }
err_return:
    for(x = 0; x < repeat; x++)
        free(value[x]);
    free(value);
    return err;
}

int extfits_get_element_size(int typecode)
{
    int typesize = 1;

    switch(typecode)
    {
        case TSHORT:
        case TUSHORT:
            typesize *= 2;
            break;
        case TINT:
        case TUINT:
        case TFLOAT:
            typesize *= 4;
        case TLONG:
        case TULONG:
        case TDOUBLE:
        case TCOMPLEX:
            typesize *= 8;
            break;
        case TDBLCOMPLEX:
            typesize *= 16;
            break;
        default:
            break;
    }

    return typesize;
}

int extfits_read_img(fitsfile * fptr, dsp_stream_p stream, int bpp, int nelements)
{
    int status = 0;
    int anynul = 0;
    void *array = malloc((size_t)(abs(bpp) * nelements / 8));
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
        return -1;
    return 0;
}

int extfits_read_col(fitsfile * fptr, dsp_stream_p stream, int n, int rown, int typecode, long repeat, long width, int nelements)
{
    int status = 0;
    int anynul = 0;
    void *array = malloc((size_t)(extfits_get_element_size(typecode) * nelements * repeat * width));
    switch(typecode)
    {
        case TBIT:
        case TBYTE:
        case TSBYTE:
        case TLOGICAL:
        case TSTRING:
            fits_read_col(fptr, TBYTE, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_stretch(((unsigned char*)(array)), (long)nelements, 0, dsp_t_max);
            dsp_buffer_copy(((unsigned char*)array), stream->buf, nelements);
            break;
        case TSHORT:
            fits_read_col(fptr, TSHORT, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((short*)array), stream->buf, nelements);
            break;
        case TUSHORT:
            fits_read_col(fptr, TUSHORT, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((unsigned short*)array), stream->buf, nelements);
            break;
        case TINT:
            fits_read_col(fptr, TINT, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((int*)array), stream->buf, nelements);
            break;
        case TUINT:
            fits_read_col(fptr, TUINT, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((unsigned int*)array), stream->buf, nelements);
            break;
        case TLONG:
            fits_read_col(fptr, TLONG, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((long*)array), stream->buf, nelements);
            break;
        case TULONG:
            fits_read_col(fptr, TULONG, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((unsigned long*)array), stream->buf, nelements);
            break;
        case TFLOAT:
            fits_read_col(fptr, TFLOAT, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((float*)array), stream->buf, nelements);
            break;
        case TDOUBLE:
            fits_read_col(fptr, TDOUBLE, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((double*)array), stream->buf, nelements);
            break;
        case TCOMPLEX:
            fits_read_col(fptr, TCOMPLEX, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((float*)array), stream->dft.buf, nelements*2);
            break;
        case TDBLCOMPLEX:
            fits_read_col(fptr, TDBLCOMPLEX, n, rown, 1, (long)nelements, NULL, array, &anynul, &status);
            dsp_buffer_copy(((double*)array), stream->dft.buf, nelements*2);
            break;
    }
    free(array);
    if(status || anynul)
        return -1;
    return 0;
}

dsp_stream_p extfits_read_fits(char *filename)
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
    if(extfits_read_img(fptr, stream, bpp, nelements))
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

dsp_stream_p* extfits_read_sdfits(char *filename, long *nstreams)
{
    fitsfile *fptr = (fitsfile*)malloc(sizeof(fitsfile));
    memset(fptr, 0, sizeof(fitsfile));
    int status = 0;
    int sdfits_hdu = 0;
    long nrows = 0;
    long nrow = 0;
    int ncols = 0;
    int nfield = 0;
    int k = 0;
    int i = 0;
    int n = 0;
    int r = 0;
    int dims;
    int dim, nelements = 1;
    int anynul = 0;
    long naxes[3] = { 1, 1, 1 };
    fitsext_column* columns = (fitsext_column*)malloc(sizeof(fitsext_column));
    char value[150];
    char comment[150];
    char error_status[64];
    dsp_stream_p *stream = NULL;

    fits_open_file(&fptr, filename, READONLY, &status);
    if (status)
    {
        goto fail;
    }

    ffgkey(fptr, FITS_KEYWORD_EXTEND.name, value, comment, &status);
    if(status || strcmp(value, FITS_KEYWORD_EXTEND.value))
    {
        goto fail;
    }

    ffgkey(fptr, SDFITS_KEYWORD_TELESCOP.name, value, comment, &status);
    if (!status)
    {
    }
    status = 0;

    ffgkey(fptr, SDFITS_KEYWORD_OBSERVER.name, value, comment, &status);
    if (!status)
    {
    }
    status = 0;

    ffgkey(fptr, SDFITS_KEYWORD_DATE_OBS.name, value, comment, &status);
    if (!status)
    {
    }
    status = 0;

    ffgkey(fptr, SDFITS_KEYWORD_DATAMAX.name, value, comment, &status);
    if (!status)
    {
    }
    status = 0;

    ffgkey(fptr, SDFITS_KEYWORD_DATAMIN.name, value, comment, &status);
    if (!status)
    {
    }
    status = 0;

    fits_movabs_hdu(fptr, 1, &sdfits_hdu, &status);
    if(status || sdfits_hdu != BINARY_TBL)
    {
        goto fail;
    }

    fits_read_key_str(fptr, "EXTNAME", value, comment, &status);
    if(status || strcmp(value, FITS_TABLE_SDFITS))
    {
        goto fail;
    }

    fits_read_key_str(fptr, SDFITS_KEYWORD_NMATRIX.name, value, NULL, &status);
    if(status || strcmp(value, SDFITS_KEYWORD_NMATRIX.value)) {
        goto fail;
    }

    fits_get_num_rows(fptr, &nrows, &status);
    if(status)
    {
        goto fail;
    }

    stream = (dsp_stream_p*)realloc(stream, sizeof(dsp_stream_p)*(size_t)nrows);

    fits_get_num_cols(fptr, &ncols, &status);
    if(status)
    {
        goto fail;
    }
    columns = (fitsext_column*)realloc(columns, sizeof(fitsext_column)*((size_t)ncols+1));
    for(r = 0; r < nrows; r++) {
        for(k = 0; k < ncols; k++) {
            columns[k].name = (char*)malloc(150);
            columns[k].format = (char*)malloc(150);
            columns[k].unit = (char*)malloc(150);
            columns[k].value = (char*)malloc(150);
            columns[k].comment = (char*)malloc(150);
            columns[k].typecode = 0;
            columns[k].repeat = 0;
            columns[k].width = 0;
            status = 0;
            for(i = 0; strcmp(SDFITS_TABLE_MAIN[i].name, ""); i++)
            {
                fits_get_colname(fptr, 0, SDFITS_TABLE_MAIN[i].name, value, &n, &status);
                if(!status && strcmp(SDFITS_TABLE_MAIN[i].name, "")) {
                    strcpy(columns[nfield].name, value);
                    fits_get_eqcoltype(fptr, n, &columns[nfield].typecode, &columns[nfield].repeat, &columns[nfield].width, &status);
                    if(status) continue;
                    if(extfits_check_column(fptr, columns[nfield].name, columns[nfield].expected, 0, r))
                        continue;
                    if(!status && dims > 0 && !strcasecmp(value, "DATA")) {
                        fits_read_tdim(fptr, n, 2, &dims, naxes, &status);
                        stream[nrow] = dsp_stream_new();
                        for(dim = 0; dim < dims; dim++)
                        {
                            dsp_stream_add_dim(stream[nrow], (int)naxes[dim]);
                        }
                        dsp_stream_alloc_buffer(stream[nrow], stream[nrow]->len);
                        nelements = stream[nrow]->len;
                        extfits_read_col(fptr, stream[nrow], n, r, columns[nfield].typecode, columns[nfield].repeat, columns[nfield].width, nelements);
                        if(status || anynul) {
                            dsp_stream_free_buffer(stream[nrow]);
                            dsp_stream_free(stream[nrow]);
                            continue;
                        }
                        nrow++;
                    }
                    nfield++;
                }
            }
        }
    }
    *nstreams = nrow;
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
