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
    long r = extfits_alloc_fits_rows(fptr, num_rows);
    fits_get_colnum(fptr, CASESEN, (char*)(name), &ncol, &status);
    if(status != COL_NOT_FOUND)
    {
        fits_get_eqcoltype(fptr, ncol, &typecode, &repeat, &width, &status);
        fits_write_col(fptr, typecode, ncol, r, 1, num_rows, buf, &status);
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

int extfits_get_value(fitsfile *fptr, char* column, long rown, void **retval)
{
    int err = 1, n = 0, anynul = 0, status = 0, typecode;
    long repeat = 1;
    long width;
    char name[64];
    if(column == NULL)
        goto err_return;
    fits_get_colname(fptr, 0, column, name, &n, &status);
    if(status)
        goto err_return;
    fits_get_coltype(fptr, n, &typecode, &repeat, &width, &status);
    void *value = malloc(extfits_get_element_size(typecode)*(size_t)(repeat*width));
    fits_read_col(fptr, typecode, n, rown, 1, repeat, NULL, value, &anynul, &status);
    *retval = value;
err_return:
    return err;
}

int extfits_check_column(fitsfile *fptr, char* column, char **expected, long rown)
{
    int err = 1, n = 0, anynul = 0, status = 0, x, y, typecode;
    long repeat = 1;
    long width;
    char name[64];
    if(column == NULL || expected == NULL)
        goto err_return;
    fits_get_colname(fptr, 0, column, name, &n, &status);
    if(status)
        goto err_return;
    fits_get_coltype(fptr, n, &typecode, &repeat, &width, &status);
    if(typecode != TSTRING)
        goto err_return;
    char **value = (char **)malloc(sizeof(char*)*(size_t)repeat);
    for(x = 0; x < repeat; x++) {
        value[x] = (char*) malloc((size_t)width);
        fits_read_col_str(fptr, n, rown, 1, 1, NULL, value, &anynul, &status);
        for(y = 0; strcmp(expected[y], ""); y++) {
            if(!strcmp(value[x], expected[y])) {
                err &= 1;
                break;
            }
        }
        if(y == 0)
            err = 0;
    }
    for(x = 0; x < repeat; x++)
        free(value[x]);
    free(value);
err_return:
    return err;
}

int extfits_check_key(fitsfile *fptr, char* keyname, char **expected)
{
    int err = 1, status = 0, y;
    char value[64];
    if(keyname == NULL || expected == NULL)
        goto err_return;
    fits_read_key_str(fptr, keyname, value, NULL, &status);
    if(status)
        goto err_return;
    for(y = 0; strcmp(expected[y], ""); y++) {
        if(!strcmp(value, expected[y])) {
            err &= 1;
            break;
        }
    }
    if(y == 0)
        err = 0;
err_return:
    return err;
}

int extfits_read_typecode(char* typestr, int *typecode, long *width, long *repeat)
{
    int w, r, t;
    char c;
    sscanf(typestr, "%d%c%d", &w, &c, &r);
    t = (int)c;
    if (t == EXTFITS_ELEMENT_BIT.typestr[0])
        t = EXTFITS_ELEMENT_BIT.typecode;
    if (t == EXTFITS_ELEMENT_STRING.typestr[0])
        t = EXTFITS_ELEMENT_STRING.typecode;
    if (t == EXTFITS_ELEMENT_LOGICAL.typestr[0])
        t = EXTFITS_ELEMENT_LOGICAL.typecode;
    if (t == EXTFITS_ELEMENT_BYTE.typestr[0])
        t = EXTFITS_ELEMENT_BYTE.typecode;
    if (t == EXTFITS_ELEMENT_SBYTE.typestr[0])
        t = EXTFITS_ELEMENT_SBYTE.typecode;
    if (t == EXTFITS_ELEMENT_SHORT.typestr[0])
        t = EXTFITS_ELEMENT_SHORT.typecode;
    if (t == EXTFITS_ELEMENT_USHORT.typestr[0])
        t = EXTFITS_ELEMENT_USHORT.typecode;
    if (t == EXTFITS_ELEMENT_INT.typestr[0])
        t = EXTFITS_ELEMENT_INT.typecode;
    if (t == EXTFITS_ELEMENT_UINT.typestr[0])
        t = EXTFITS_ELEMENT_UINT.typecode;
    if (t == EXTFITS_ELEMENT_LONG.typestr[0])
        t = EXTFITS_ELEMENT_LONG.typecode;
    if (t == EXTFITS_ELEMENT_FLOAT.typestr[0])
        t = EXTFITS_ELEMENT_FLOAT.typecode;
    if (t == EXTFITS_ELEMENT_DOUBLE.typestr[0])
        t = EXTFITS_ELEMENT_DOUBLE.typecode;
    if (t == EXTFITS_ELEMENT_COMPLEX.typestr[0])
        t = EXTFITS_ELEMENT_COMPLEX.typecode;
    if (t == EXTFITS_ELEMENT_DBLCOMPLEX.typestr[0])
        t = EXTFITS_ELEMENT_DBLCOMPLEX.typecode;
    else return -1;
    *typecode = t;
    *width = w;
    *repeat = r;
    return 0;
}

size_t extfits_get_element_size(int typecode)
{
    size_t typesize = 1;

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
            break;
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

dsp_stream_p * vlbi_file_read_sdfits(char * filename, long *n)
{
    long nrows = 0;
    long dim;
    long dims, *sizes;
    int typecode = 0;
    long width = 0;
    long repeat = 0;
    int r = 0;
    extfits_row *rows = extfits_read_sdfits(filename, &nrows, &dims, &sizes);
    dsp_stream_p *stream = (dsp_stream_p*)malloc(sizeof(dsp_stream_p)*(size_t)nrows);
    for(r = 0; r < nrows; r++) {
        stream[r] = dsp_stream_new();
        for(dim = 0; dim < dims; dim++)
        {
            dsp_stream_add_dim(stream[r], (int)sizes[dim]);
        }
        int k = 0;
        extfits_row row = rows[r];
        for(k = 0; strcmp(SDFITS_COLUMN_OBJCTRA.name, row.columns[k].name); k++);
        f_scansexa(row.columns[k].value, &stream[r]->target[0]);
        for(k = 0; strcmp(SDFITS_COLUMN_OBJCTDEC.name, row.columns[k].name); k++);
        f_scansexa(row.columns[k].value, &stream[r]->target[1]);
        for(k = 0; strcmp(SDFITS_COLUMN_OBJCTDEC.name, row.columns[k].name); k++);
        f_scansexa(row.columns[k].value, &stream[r]->target[1]);
        for(k = 0; strcmp(SDFITS_COLUMN_OBSFREQ.name, row.columns[k].name); k++);
        stream[r]->wavelength = *((double*)row.columns[k].value);
        stream[r]->wavelength = LIGHTSPEED/stream[r]->wavelength;
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
        double time = (long)*((double*)row.columns[k].value);
        time -= stream[r]->samplerate / 2.0;
        stream[r]->starttimeutc.tv_sec += (long)time;
        stream[r]->starttimeutc.tv_nsec += (long)((time-stream[r]->starttimeutc.tv_sec)*1000000000);

        for(k = 0; strcmp(SDFITS_COLUMN_DATA.name, row.columns[k].name); k++);

        extfits_read_typecode(row.columns[k].format, &typecode, &width, &repeat);
        dsp_stream_add_dim(stream[r], (int)width);
        dsp_stream_add_dim(stream[r], (int)repeat);
        dsp_stream_alloc_buffer(stream[r], stream[r]->len);

        stream[r]->samplerate /= stream[r]->len;
        stream[r]->samplerate = 1.0/stream[r]->samplerate;
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
                dsp_buffer_copy(((float*)row.columns[k].value), stream[r]->dft.buf, stream[r]->len*2);
                break;
            case TDBLCOMPLEX:
                dsp_buffer_copy(((double*)row.columns[k].value), stream[r]->dft.buf, stream[r]->len*2);
                break;
        default:break;
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

extfits_row* extfits_read_sdfits(char *filename, long *num_rows, long *maxes, long **maxis)
{
    fitsfile *fptr = (fitsfile*)malloc(sizeof(fitsfile));
    memset(fptr, 0, sizeof(fitsfile));
    int status = 0;
    int sdfits_hdu = 0;
    long nrows = 0;
    long r = 0;
    long nmatrix = 0;
    int ncols = 0;
    int typecode = 0;
    int width = 0;
    int repeat = 0;
    int k = 0;
    int i = 0;
    int n = 0;
    int dim;
    int anynul = 0;
    long naxes[3] = { 1, 1, 1 };
    extfits_column* columns = (extfits_column*)malloc(sizeof(extfits_column));
    extfits_row* rows = (extfits_row*)malloc(sizeof(extfits_row));
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

    fits_read_key_str(fptr, EXTFITS_KEYWORD_NMATRIX.name, value, NULL, &status);
    if(status || strcmp(value, EXTFITS_KEYWORD_NMATRIX.value)) {
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

    fits_read_key_lng(fptr, EXTFITS_KEYWORD_NMATRIX.name, &nmatrix, NULL, &status);
    if(status || nmatrix < 1)
    {
        goto fail;
    }

    columns = (extfits_column*)realloc(columns, sizeof(extfits_column)*((size_t)ncols+1));
    rows = (extfits_row*)realloc(rows, sizeof(extfits_row)*((size_t)nrows+1));

    for(r = 0; r < nrows; r++) {
        for(k = 0; k < ncols; k++) {
            columns[k].name = (char*)malloc(150);
            columns[k].format = (char*)malloc(150);
            columns[k].unit = (char*)malloc(150);
            columns[k].value = (char*)malloc(150);
            columns[k].comment = (char*)malloc(150);

            status = 0;
            fits_get_colname(fptr, 0, SDFITS_TABLE_MAIN[i].name, value, &n, &status);
            strcpy(columns[k].name, value);
            if(!extfits_check_key(fptr, EXTFITS_KEYWORD_TMATX(k).name, &EXTFITS_KEYWORD_TMATX(k).value)) {
                int max_dims = 5;
                int dims;
                long *sizes =(long*)malloc(sizeof(long)*(size_t)max_dims);
                fits_read_tdim(fptr, k, max_dims, &dims, sizes, &status);
                if(dims < 2) {
                    long d = 0;
                    fits_read_key_lng(fptr, EXTFITS_KEYWORD_MAXIS().name, &d, NULL, &status);
                    sizes = (long*)malloc(sizeof(long)*(size_t)dims);
                    for(dim = 0; dim < d; dim++)
                        fits_read_key_lng(fptr, EXTFITS_KEYWORD_MAXIS(dim).name, &sizes[dim], NULL, &status);
                    dims = (int)d;
                }
                if(dims > 0) {
                    void *tcs = NULL;
                    extfits_get_value(fptr, EXTFITS_KEYWORD_TMATX(k).axes_definition.format.name, r, &tcs);
                    strcpy(columns[k].format, tcs);
                    extfits_get_value(fptr, EXTFITS_KEYWORD_TMATX(k).axes_definition.unit.name, r, &tcs);
                    strcpy(columns[k].unit, tcs);
                    if (!extfits_read_typecode((char*)tcs, &typecode, &width, &repeat)) {
                        size_t element_size = extfits_get_element_size(typecode);
                        long nelements = 1;
                        for(dim = 0; dim < dims; dim++) {
                            nelements *= naxes[dim];
                        }
                        columns[k].value = (char*)malloc(element_size*(size_t)nelements);
                        fits_read_col(fptr, typecode, k, r, 1, nelements, NULL, columns->value, &anynul, &status);
                        if(!anynul && !status) {
                            *maxis = (long*)malloc(sizeof(long)*(size_t)dims);
                            for(dim = 0; dim < dims; dim++)
                                *maxis[dim] = naxes[dim];
                            *maxes = dims;
                        }
                    }
                }
            } else {
                int typecode;
                long repeat, width;
                fits_get_eqcoltype(fptr, n, &typecode, &repeat, &width, &status);
                if(status) continue;
                if(extfits_check_column(fptr, columns[k].name, columns[k].expected, r))
                    continue;
                void *val = &columns[k].value;
                extfits_get_value(fptr, columns[k].name, r, &val);
            }
        }
        rows[r].columns = (extfits_column*)malloc(sizeof(extfits_column)*rows[r].num_columns);
        rows[r].num_columns = (size_t)ncols;
    }
    *num_rows = nrows;
    status = 0;
    fits_close_file(fptr, &status);
    if(status)
        goto fail;
    return rows;
fail:
    free(rows);
    free(columns);
    if(status)
    {
        fits_report_error(stderr, status); /* print out any error messages */
        fits_get_errstatus(status, error_status);
        fprintf(stderr, "FITS Error: %s\n", error_status);
    }
    return NULL;
}
