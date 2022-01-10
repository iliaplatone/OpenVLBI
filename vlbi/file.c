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
