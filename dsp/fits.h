/*
*   DSP API - a digital signal processing library for astronomy usage
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

#ifndef _DSP_FITS_H
#define _DSP_FITS_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef DLL_EXPORT
#define DLL_EXPORT extern
#endif

#include <fitsio.h>

typedef struct
{
    char typestr[8];
    int typecode;
    long repeat;
    long width;
} dsp_fits_format;

typedef struct
{
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    char ** expected;
} dsp_fits_keyword;

typedef struct
{
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    char ** expected;
} dsp_fits_column;

typedef struct
{
    dsp_fits_column* columns;
    size_t num_columns;
} dsp_fits_row;

typedef struct
{
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    struct
    {
        dsp_fits_keyword name;
        dsp_fits_keyword increment;
        dsp_fits_keyword refpix;
        dsp_fits_keyword value;
    } definition;
} dsp_fits_axis;

typedef struct
{
    char *name;
    char *format;
    char *value;
    char *comment;
    struct
    {
        dsp_fits_keyword name;
        dsp_fits_keyword format;
        dsp_fits_keyword unit;
        dsp_fits_keyword dims;
    } axes_definition;
} dsp_fits_matrix;

///Returns non-zero decimal conversion of integer into string
#ifndef itostr
#define its(x) #x
#define itostr(x) its(x)
#endif

///FITS element types
#define EXTFITS_ELEMENT_STRING (dsp_fits_format){"A", TSTRING, 0, 0}
#define EXTFITS_ELEMENT_LOGICAL (dsp_fits_format){"L", TLOGICAL, 0, 0}
#define EXTFITS_ELEMENT_BIT (dsp_fits_format){"X", TBIT, 0, 0}
#define EXTFITS_ELEMENT_BYTE (dsp_fits_format){"B", TBYTE, 0, 0}
#define EXTFITS_ELEMENT_SBYTE (dsp_fits_format){"S", TSBYTE, 0, 0}
#define EXTFITS_ELEMENT_SHORT (dsp_fits_format){"I", TSHORT, 0, 0}
#define EXTFITS_ELEMENT_USHORT (dsp_fits_format){"U", TUSHORT, 0, 0}
#define EXTFITS_ELEMENT_INT (dsp_fits_format){"J", TINT, 0, 0}
#define EXTFITS_ELEMENT_UINT (dsp_fits_format){"V", TUINT, 0, 0}
#define EXTFITS_ELEMENT_LONG (dsp_fits_format){"K", TLONG, 0, 0}
#define EXTFITS_ELEMENT_FLOAT (dsp_fits_format){"E", TFLOAT, 0, 0}
#define EXTFITS_ELEMENT_DOUBLE (dsp_fits_format){"D", TDOUBLE, 0, 0}
#define EXTFITS_ELEMENT_COMPLEX (dsp_fits_format){"C", TCOMPLEX, 0, 0}
#define EXTFITS_ELEMENT_DBLCOMPLEX (dsp_fits_format){"M", TDBLCOMPLEX, 0, 0}

///FITS Measure units
#define EXTFITS_MEASURE_UNIT_HZ "Hz"
#define EXTFITS_MEASURE_UNIT_SECOND "sec"
#define EXTFITS_MEASURE_UNIT_MINUTE "min"
#define EXTFITS_MEASURE_UNIT_HOUR "hour"
#define EXTFITS_MEASURE_UNIT_DAY "day"
#define EXTFITS_MEASURE_UNIT_MONTH "month"
#define EXTFITS_MEASURE_UNIT_YEAR "year"
#define EXTFITS_MEASURE_UNIT_JANSKY "Jy"
#define EXTFITS_MEASURE_UNIT_KELVIN "K"
#define EXTFITS_MEASURE_UNIT_ANGSTROM "Angstrom"
#define EXTFITS_MEASURE_UNIT_ARCSEC "arcsec"
#define EXTFITS_MEASURE_UNIT_ARCMIN "arcmin"
#define EXTFITS_MEASURE_UNIT_DEGREE "degree"
#define EXTFITS_MEASURE_UNIT_PERCENT "percent"
#define EXTFITS_MEASURE_UNIT_METER "meter"
#define EXTFITS_MEASURE_UNIT_MILLIBAR "millibar"

///Set to 'FLUX' or 'DATA' for matrix buffers
#define EXTFITS_KEYWORD_TTYPE(n) (dsp_fits_keyword){"TTYPE" itostr(n), "8A", "", "", "Set to 'FLUX'", (char*[]){"FLUX", "DATA", ""}}
///shall have the value 'K', 'JY' or 'UNCALIB'
#define EXTFITS_KEYWORD_TUNIT(n) (dsp_fits_keyword){"TUNIT" itostr(n), "8A", "", "", "Shall have the value 'JY' or 'UNCALIB'", (char*[]){""}}
///Size in pixels of data buffer
#define EXTFITS_KEYWORD_TDIM(n) (dsp_fits_keyword){"TDIM" itostr(n), "8A", "", "", "Size in pixels of data buffer", (char*[]){""}}
///shall have the format of the column
#define EXTFITS_KEYWORD_TFORM(n) (dsp_fits_keyword){"TFORM" itostr(n), "8A", "", "", "Shall be a character string", (char*[]){""}}

///Name of regular axis m = 1 to M
#define EXTFITS_KEYWORD_CTYPE(m) (dsp_fits_keyword){"CTYPE" itostr(m), EXTFITS_ELEMENT_STRING.typestr, "", "", "Name of regular axis m = 1 to M", (char*[]){""}}
///Coordinate increment on axis m = 1 to M
#define EXTFITS_KEYWORD_CDELT(m) (dsp_fits_keyword){"CDELT" itostr(m), EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Coordinate increment on axis m = 1 to M", (char*[]){""}}
///Reference pixel on axis m = 1 to M
#define EXTFITS_KEYWORD_CRPIX(m) (dsp_fits_keyword){"CRPIX" itostr(m), EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Reference pixel on axis m = 1 to M", (char*[]){""}}
///Coordinate value at reference pixel on axis m = 1 to M
#define EXTFITS_KEYWORD_CRVAL(m) (dsp_fits_keyword){"CRVAL" itostr(m), EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Coordinate value at reference pixel on axis m = 1 to M", (char*[]){""}}

///NMATRIX shall be present with the value 1
#define EXTFITS_KEYWORD_NMATRIX (dsp_fits_keyword){"NMATRIX", EXTFITS_ELEMENT_SHORT.typestr, "", "1", "NMATRIX shall be present with the value 1", (char*[]){"1", ""}}
///Set to 'T' — column n contains the visibility matrix
#define EXTFITS_KEYWORD_TMATX(n) (dsp_fits_matrix){"TMATX" itostr(n), "8A", "T", "Set to 'T'", {EXTFITS_KEYWORD_TTYPE(n), EXTFITS_KEYWORD_TFORM(n), EXTFITS_KEYWORD_TUNIT(n), EXTFITS_KEYWORD_TDIM(n)}}
///M = number axes in regular matrix, Number pixels on axis m = 1 to M
#define EXTFITS_KEYWORD_MAXIS(m) (dsp_fits_axis){"MAXIS" itostr(m), EXTFITS_ELEMENT_SHORT.typestr, "", "", "M = number axes in regular matrix, Number pixels on axis m = 1 to M", {EXTFITS_KEYWORD_CTYPE(m), EXTFITS_KEYWORD_CDELT(m), EXTFITS_KEYWORD_CRPIX(m), EXTFITS_KEYWORD_CRVAL(m)}}

///Target right ascension coordinate
#define EXTFITS_KEYWORD_OBJCTRA (dsp_fits_column){"OBJCTRA", EXTFITS_ELEMENT_STRING.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Target right ascension coordinate", (char*[]){""}}
///Target declination coordinate
#define EXTFITS_KEYWORD_OBJCTDEC (dsp_fits_column){"OBJCTDEC", EXTFITS_ELEMENT_STRING.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Target declination coordinate", (char*[]){""}}

#define FITS_KEYWORD_EXTEND (dsp_fits_keyword){"EXTEND", "A", "", "T", "", (char*[]){""}}

void dsp_fits_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status);
long dsp_fits_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows);
int dsp_fits_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, long num_elements, unsigned long rown);
int dsp_fits_append_fits_col(fitsfile *fptr, char* name, char* format);
void dsp_fits_delete_fits_col(fitsfile *fptr, char* name);
size_t dsp_fits_get_element_size(int typecode);
int dsp_fits_read_typecode(char* typestr, int *typecode, long *width, long *repeat);
int dsp_fits_get_value(fitsfile *fptr, char* column, long rown, void **retval);
int dsp_fits_check_column(fitsfile *fptr, char* column, char **expected, long rown);
fitsfile* dsp_fits_create_fits_(size_t *size, void **buf);
int dsp_fits_close_fits(fitsfile *fptr);

#ifdef __cplusplus
}
#endif

#endif //_DSP_FITS_H
