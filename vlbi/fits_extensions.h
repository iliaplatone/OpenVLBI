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

#ifndef _FITS_EXTENSIONS_H
#define _FITS_EXTENSIONS_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef DLL_EXPORT
#define DLL_EXPORT extern
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vlbi.h>
#include <fitsio.h>

typedef struct
{
    char typestr[8];
    int typecode;
    long repeat;
    long width;
} extfits_format;

typedef struct
{
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    char ** expected;
} extfits_keyword;

typedef struct
{
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    char ** expected;
} extfits_column;

typedef struct
{
    extfits_column* columns;
    size_t num_columns;
} extfits_row;

typedef struct
{
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    struct
    {
        extfits_keyword name;
        extfits_keyword increment;
        extfits_keyword refpix;
        extfits_keyword value;
    } definition;
} extfits_axis;

typedef struct
{
    char *name;
    char *format;
    char *value;
    char *comment;
    struct
    {
        extfits_keyword name;
        extfits_keyword format;
        extfits_keyword unit;
        extfits_keyword dims;
    } axes_definition;
} extfits_matrix;

///Returns non-zero decimal conversion of integer into string
#ifndef itostr
#define its(x) #x
#define itostr(x) its(x)
#endif

///Antenna polarizations
///I
#define EXTFITS_STOKE_I "1"
///Q
#define EXTFITS_STOKE_Q "2"
///U
#define EXTFITS_STOKE_U "3"
///V
#define EXTFITS_STOKE_V "4"
///RR
#define EXTFITS_STOKE_RR "-1"
///LL
#define EXTFITS_STOKE_LL "-2"
///RL
#define EXTFITS_STOKE_RL "-3"
///LR
#define EXTFITS_STOKE_LR "-4"
///XX
#define EXTFITS_STOKE_XX "-5"
///YY
#define EXTFITS_STOKE_YY "-6"
///XY
#define EXTFITS_STOKE_XY "-7"
///YX
#define EXTFITS_STOKE_YX "-8"

///FITS element types
#define EXTFITS_ELEMENT_STRING (extfits_format){"A", TSTRING, 0, 0}
#define EXTFITS_ELEMENT_LOGICAL (extfits_format){"L", TLOGICAL, 0, 0}
#define EXTFITS_ELEMENT_BIT (extfits_format){"X", TBIT, 0, 0}
#define EXTFITS_ELEMENT_BYTE (extfits_format){"B", TBYTE, 0, 0}
#define EXTFITS_ELEMENT_SBYTE (extfits_format){"S", TSBYTE, 0, 0}
#define EXTFITS_ELEMENT_SHORT (extfits_format){"I", TSHORT, 0, 0}
#define EXTFITS_ELEMENT_USHORT (extfits_format){"U", TUSHORT, 0, 0}
#define EXTFITS_ELEMENT_INT (extfits_format){"J", TINT, 0, 0}
#define EXTFITS_ELEMENT_UINT (extfits_format){"V", TUINT, 0, 0}
#define EXTFITS_ELEMENT_LONG (extfits_format){"K", TLONG, 0, 0}
#define EXTFITS_ELEMENT_FLOAT (extfits_format){"E", TFLOAT, 0, 0}
#define EXTFITS_ELEMENT_DOUBLE (extfits_format){"D", TDOUBLE, 0, 0}
#define EXTFITS_ELEMENT_COMPLEX (extfits_format){"C", TCOMPLEX, 0, 0}
#define EXTFITS_ELEMENT_DBLCOMPLEX (extfits_format){"M", TDBLCOMPLEX, 0, 0}

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
#define EXTFITS_KEYWORD_TTYPE(n) (extfits_keyword){"TTYPE" itostr(n), "8A", "", "", "Set to 'FLUX'", (char*[]){"FLUX", "DATA", ""}}
///shall have the value 'K', 'JY' or 'UNCALIB'
#define EXTFITS_KEYWORD_TUNIT(n) (extfits_keyword){"TUNIT" itostr(n), "8A", "", "", "Shall have the value 'JY' or 'UNCALIB'", (char*[]){""}}
///Size in pixels of data buffer
#define EXTFITS_KEYWORD_TDIM(n) (extfits_keyword){"TDIM" itostr(n), "8A", "", "", "Size in pixels of data buffer", (char*[]){""}}
///shall have the format of the column
#define EXTFITS_KEYWORD_TFORM(n) (extfits_keyword){"TFORM" itostr(n), "8A", "", "", "Shall be a character string", (char*[]){""}}

///Name of regular axis m = 1 to M
#define EXTFITS_KEYWORD_CTYPE(m) (extfits_keyword){"CTYPE" itostr(m), EXTFITS_ELEMENT_STRING.typestr, "", "", "Name of regular axis m = 1 to M", (char*[]){""}}
///Coordinate increment on axis m = 1 to M
#define EXTFITS_KEYWORD_CDELT(m) (extfits_keyword){"CDELT" itostr(m), EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Coordinate increment on axis m = 1 to M", (char*[]){""}}
///Reference pixel on axis m = 1 to M
#define EXTFITS_KEYWORD_CRPIX(m) (extfits_keyword){"CRPIX" itostr(m), EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Reference pixel on axis m = 1 to M", (char*[]){""}}
///Coordinate value at reference pixel on axis m = 1 to M
#define EXTFITS_KEYWORD_CRVAL(m) (extfits_keyword){"CRVAL" itostr(m), EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Coordinate value at reference pixel on axis m = 1 to M", (char*[]){""}}

///NMATRIX shall be present with the value 1
#define EXTFITS_KEYWORD_NMATRIX (extfits_keyword){"NMATRIX", EXTFITS_ELEMENT_SHORT.typestr, "", "1", "NMATRIX shall be present with the value 1", (char*[]){"1", ""}}
///Set to 'T' — column n contains the visibility matrix
#define EXTFITS_KEYWORD_TMATX(n) (extfits_matrix){"TMATX" itostr(n), "8A", "T", "Set to 'T'", {EXTFITS_KEYWORD_TTYPE(n), EXTFITS_KEYWORD_TFORM(n), EXTFITS_KEYWORD_TUNIT(n), EXTFITS_KEYWORD_TDIM(n)}}
///M = number axes in regular matrix, Number pixels on axis m = 1 to M
#define EXTFITS_KEYWORD_MAXIS(m) (extfits_axis){"MAXIS" itostr(m), EXTFITS_ELEMENT_SHORT.typestr, "", "", "M = number axes in regular matrix, Number pixels on axis m = 1 to M", {EXTFITS_KEYWORD_CTYPE(m), EXTFITS_KEYWORD_CDELT(m), EXTFITS_KEYWORD_CRPIX(m), EXTFITS_KEYWORD_CRVAL(m)}}

///Target right ascension coordinate
#define EXTFITS_KEYWORD_OBJCTRA (extfits_column){"OBJCTRA", EXTFITS_ELEMENT_STRING.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Target right ascension coordinate", (char*[]){""}}
///Target declination coordinate
#define EXTFITS_KEYWORD_OBJCTDEC (extfits_column){"OBJCTDEC", EXTFITS_ELEMENT_STRING.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Target declination coordinate", (char*[]){""}}

///FITS-IDI Convention Tables
///Antenna polarization information
#define FITS_TABLE_FITSIDI_ANTENNA "ANTENNA"
///Time system information and antenna coordinates
#define FITS_TABLE_FITSIDI_ARRAY_GEOMETRY "ARRAY_GEOMETRY"
///Channel-dependent complex gains
#define FITS_TABLE_FITSIDI_BANDPASS "BANDPASS"
///Baseline-specific gain factors
#define FITS_TABLE_FITSIDI_BASELINE "BASELINE"
///Complex gains as a function of time
#define FITS_TABLE_FITSIDI_CALIBRATION "CALIBRATION"
///Information for flagging data
#define FITS_TABLE_FITSIDI_FLAG "FLAG"
///Frequency setups
#define FITS_TABLE_FITSIDI_FREQUENCY "FREQUENCY"
///Antenna gain curves
#define FITS_TABLE_FITSIDI_GAIN_CURVE "GAIN_CURVE"
///Correlator model parameters
#define FITS_TABLE_FITSIDI_INTERFEROMETER_MODEL "INTERFEROMETER_MODEL"
///Phase cal measurements
#define FITS_TABLE_FITSIDI_PHASE_CAL "PHASE-CAL"
///Information on sources observed
#define FITS_TABLE_FITSIDI_SOURCE "SOURCE"
///System and antenna temperatures
#define FITS_TABLE_FITSIDI_SYSTEM_TEMPERATURE "SYSTEM_TEMPERATURE"
///Visibility data
#define FITS_TABLE_FITSIDI_UV_DATA "UV_DATA"
///Meteorological data
#define FITS_TABLE_FITSIDI_WEATHER "WEATHER"

///FITS-IDI global keywords
///Name/type of correlator
#define FITSIDI_COLUMN_CORRELAT (extfits_column){"CORRELAT", EXTFITS_ELEMENT_STRING.typestr, "", "", "Name/type of correlator", (char*[]){""}}
///Version number of the correlator software that produced the file
#define FITSIDI_COLUMN_FXCORVER (extfits_column){"FXCORVER", EXTFITS_ELEMENT_STRING.typestr, "", "", "Version number of the correlator software that produced the file", (char*[]){""}}

///FITS-IDI common table keywords
///Revision number of the table definition
#define FITSIDI_KEYWORD_TABREV "TABREV"
///Observation identification
#define FITSIDI_KEYWORD_OBSCODE "OBSCODE"
///The number of Stokes parameters
#define FITSIDI_KEYWORD_NO_STKD "NO_STKD"
///The first Stokes parameter coordinate value
#define FITSIDI_KEYWORD_STK_1 "STK_1"
///The number of bands
#define FITSIDI_KEYWORD_NO_BAND "NO_BAND"
///The number of spectral channels per band
#define FITSIDI_KEYWORD_NO_CHAN "NO_CHAN"
///The file reference frequency in Hz
#define FITSIDI_KEYWORD_REF_FREQ "REF_FREQ"
///The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1
#define FITSIDI_KEYWORD_CHAN_BW "CHAN_BW"
///The reference pixel for the frequency axis
#define FITSIDI_KEYWORD_REF_PIXL "REF_PIXL"

///Regular axes for the FITS-IDI UV_DATA table data matrix
///Real, imaginary, weight
#define FITSIDI_UV_DATA_AXIS_COMPLEX (extfits_column){"COMPLEX", "", "", "", "Real, imaginary, weight", (char*[]){""}}
///Stokes parameter
#define FITSIDI_UV_DATA_AXIS_STOKES (extfits_column){"STOKES", "", "", "", "Stokes parameter", (char*[]){""}}
///Frequency (spectral channel)
#define FITSIDI_UV_DATA_AXIS_FREQ (extfits_column){"FREQ", "", "", EXTFITS_MEASURE_UNIT_HZ, "Frequency (spectral channel)", (char*[]){""}}
///Band number
#define FITSIDI_UV_DATA_AXIS_BAND (extfits_column){"BAND", "", "", EXTFITS_MEASURE_UNIT_HZ, "Band number", (char*[]){""}}
///Right ascension of the phase center
#define FITSIDI_UV_DATA_AXIS_RA (extfits_column){"RA", "", "", EXTFITS_MEASURE_UNIT_DEGREE, "Right ascension of the phase center", (char*[]){""}}
///Declination of the phase center
#define FITSIDI_UV_DATA_AXIS_DEC (extfits_column){"DEC", "", "", EXTFITS_MEASURE_UNIT_DEGREE, "Declination of the phase center", (char*[]){""}}

///Random parameters for the FITS-IDI UV_DATA table
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU (extfits_column){"UU", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-SIN system)", (char*[]){""}}
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV (extfits_column){"VV", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-SIN system)", (char*[]){""}}
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW (extfits_column){"WW", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-SIN system)", (char*[]){""}}
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU_SIN (extfits_column){"UU---SIN", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-SIN system)", (char*[]){""}}
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV_SIN (extfits_column){"VV---SIN", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-SIN system)", (char*[]){""}}
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW_SIN (extfits_column){"WW---SIN", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-SIN system)", (char*[]){""}}
///seconds u baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_UU_NCP (extfits_column){"UU---NCP", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-NCP system)", (char*[]){""}}
///seconds v baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_VV_NCP (extfits_column){"VV---NCP", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-NCP system)", (char*[]){""}}
///seconds w baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_WW_NCP (extfits_column){"WW---NCP", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-NCP system)", (char*[]){""}}
///days Julian date at 0 hours
#define FITSIDI_UV_DATA_COLUMN_DATE (extfits_column){"DATE", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Julian date at 0 hours", (char*[]){""}}
///days Time elapsed since 0 hours
#define FITSIDI_UV_DATA_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Time elapsed since 0 hours", (char*[]){""}}
///Baseline number
#define FITSIDI_UV_DATA_COLUMN_BASELINE (extfits_column){"BASELINE", "1J", "", "", "Baseline number", (char*[]){""}}
///Array number
#define FITSIDI_UV_DATA_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Source ID number
#define FITSIDI_UV_DATA_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Frequency setup ID number
#define FITSIDI_UV_DATA_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup ID number", (char*[]){""}}
///seconds Integration time
#define FITSIDI_UV_DATA_COLUMN_INTTIM (extfits_column){"INTTIM", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "Integration time", (char*[]){""}}
///Weights
#define FITSIDI_UV_DATA_COLUMN_WEIGHT(nstokes, nband) (extfits_column){"WEIGHT", EXTFITS_ELEMENT_FLOAT.typestr itostr(nstokes) "," itostr(nband), "", "", "Weights", (char*[]){""}}

///Mandatory keywords for the FITS-IDI UV_DATA table
///2
#define FITSIDI_UV_DATA_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "2", (char*[]){""}}

///The number of Stokes parameters
#define FITSIDI_UV_DATA_KEYWORD_NO_STKD (extfits_keyword){"NO_STKD", EXTFITS_ELEMENT_SHORT.typestr, "", "", "The number of Stokes parameters", (char*[]){""}}
///The first Stokes parameter coordinate value
#define FITSIDI_UV_DATA_KEYWORD_STK_1 (extfits_keyword){"STK_1", EXTFITS_ELEMENT_SHORT.typestr, "", "", "The first Stokes parameter coordinate value", (char*[]){""}}
///The number of bands
#define FITSIDI_UV_DATA_KEYWORD_NO_BAND (extfits_keyword){"NO_BAND", EXTFITS_ELEMENT_SHORT.typestr, "", "", "The number of bands", (char*[]){""}}
///The number of spectral channels per band
#define FITSIDI_UV_DATA_KEYWORD_NO_CHAN (extfits_keyword){"NO_CHAN", EXTFITS_ELEMENT_SHORT.typestr, "", "", "The number of spectral channels per band", (char*[]){""}}
///The file reference frequency in Hz
#define FITSIDI_UV_DATA_KEYWORD_REF_FREQ (extfits_keyword){"REF_FREQ", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "The file reference frequency in Hz", (char*[]){""}}
///The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1
#define FITSIDI_UV_DATA_KEYWORD_CHAN_BW (extfits_keyword){"CHAN_BW", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1", (char*[]){""}}
///The reference pixel for the frequency axis
#define FITSIDI_UV_DATA_KEYWORD_REF_PIXL (extfits_keyword){"REF_PIXL", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "The reference pixel for the frequency axis", (char*[]){""}}
///Mean equinox
#define FITSIDI_UV_DATA_KEYWORD_EQUINOX (extfits_keyword){"EQUINOX", "8A", "", "", "Mean equinox", (char*[]){""}}
///Type of data weights
#define FITSIDI_UV_DATA_KEYWORD_WEIGHTYP (extfits_keyword){"WEIGHTYP", "8A", "", "", "Type of data weights", (char*[]){""}}

///Columns for the FITS-IDI ARRAY_GEOMETRY table
///Antenna name
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ANNAME (extfits_column){"ANNAME", "8A", "", "", "Antenna name", (char*[]){""}}
///meters Antenna station coordinates (x, y, z)
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STABXYZ (extfits_column){"STABXYZ", "3D", EXTFITS_MEASURE_UNIT_METER, "", "Antenna station coordinates (x, y, z)", (char*[]){""}}
///meters/s First-order derivatives of the station coordinates with respect to time
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DERXYZ (extfits_column){"DERXYZ", "3E", "meters/s", "", "First-order derivatives of the station coordinates with respect to time", (char*[]){""}}
///Orbital parameters
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ORBPARM(norb) (extfits_column){"ORBPARM", EXTFITS_ELEMENT_DOUBLE.typestr itostr(norb), "", "", "Orbital parameters", (char*[]){""}}
///Antenna number
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_NOSTA (extfits_column){"NOSTA", "1I", "", "", "Antenna number", (char*[]){""}}
///Mount type
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_MNTSTA (extfits_column){"MNTSTA", "1J", "", "", "Mount type", (char*[]){""}}
///meters Axis offset
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STAXOF (extfits_column){"STAXOF", "3E", EXTFITS_MEASURE_UNIT_METER, "", "Axis offset", (char*[]){""}}
///meters Antenna diameter
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DIAMETER (extfits_column){"DIAMETER", "1E", EXTFITS_MEASURE_UNIT_METER, "", "Antenna diameter", (char*[]){""}}

///Mandatory keywords for the FITS-IDI ARRAY_GEOMETRY table
///1
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Array number
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_EXTVER (extfits_keyword){"EXTVER", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Array number", (char*[]){""}}
///Array name
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRNAM (extfits_keyword){"ARRNAM", EXTFITS_ELEMENT_STRING.typestr, "", "", "Array name", (char*[]){""}}
///Coordinate frame
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FRAME (extfits_keyword){"FRAME", EXTFITS_ELEMENT_STRING.typestr, "", "", "Coordinate frame", (char*[]){""}}
///x coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYX (extfits_keyword){"ARRAYX", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "x coordinate of array center (m)", (char*[]){""}}
///y coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYY (extfits_keyword){"ARRAYY", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "y coordinate of array center (m)", (char*[]){""}}
///z coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYZ (extfits_keyword){"ARRAYZ", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "z coordinate of array center (m)", (char*[]){""}}
///norb= number orbital parameters in table
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_NUMORB (extfits_keyword){"NUMORB", EXTFITS_ELEMENT_SHORT.typestr, "", "", "norb= number orbital parameters in table", (char*[]){""}}
///Reference frequency (Hz)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FREQ (extfits_keyword){"FREQ", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Reference frequency (Hz)", (char*[]){""}}
///Time system
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TIMESYS (extfits_keyword){"TIMESYS", EXTFITS_ELEMENT_STRING.typestr, "", "", "Time system", (char*[]){""}}
///Reference date
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_RDATE (extfits_keyword){"RDATE", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Reference date", (char*[]){""}}
///GST at 0h on reference date (degrees)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_GSTIA0 (extfits_keyword){"GSTIA0", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "GST at 0h on reference date (degrees)", (char*[]){""}}
///Earth's rotation rate (degrees/day)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_DEGPDY (extfits_keyword){"DEGPDY", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "Earth's rotation rate (degrees/day)", (char*[]){""}}
///UT1 - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_UT1UTC (extfits_keyword){"UT1UTC", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "UT1 - UTC (sec)", (char*[]){""}}
///IAT - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_IATUTC (extfits_keyword){"IATUTC", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "IAT - UTC (sec)", (char*[]){""}}
///x coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARX (extfits_keyword){"POLARX", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "x coordinate of North Pole (arc seconds)", (char*[]){""}}
///y coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARY (extfits_keyword){"POLARY", EXTFITS_ELEMENT_FLOAT.typestr, "", "", "y coordinate of North Pole (arc seconds)", (char*[]){""}}

///Columns for the FITS-IDI ANTENNA table
///days Central time of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of period covered by record", (char*[]){""}}
///days Duration of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of period covered by record", (char*[]){""}}
///Antenna name
#define FITSIDI_ANTENNA_COLUMN_ANNAME (extfits_column){"ANNAME", "8A", "", "", "Antenna name", (char*[]){""}}
///Antenna number
#define FITSIDI_ANTENNA_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_ANTENNA_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_ANTENNA_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Number of digitizer levels
#define FITSIDI_ANTENNA_COLUMN_NO_LEVELS (extfits_column){"NO_LEVELS", "1J", "", "", "Number of digitizer levels", (char*[]){""}}
///Feed A polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYA (extfits_column){"POLTYA", "1A", "Feed A polarization label", (char*[]){""}}
///degrees Feed A orientation
#define FITSIDI_ANTENNA_COLUMN_POLAA(nband) (extfits_column){"POLAA", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_DEGREE, "", "Feed A orientation", (char*[]){""}}
///Feed A polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALA(npcal, nband) (extfits_column){"POLCALA", EXTFITS_ELEMENT_FLOAT.typestr itostr(npcal) "," itostr(nband), "", "", "Feed A polarization parameters", (char*[]){""}}
///Feed B polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYB (extfits_column){"POLTYB", "1A", "Feed B polarization label", (char*[]){""}}
///degrees Feed B orientation
#define FITSIDI_ANTENNA_COLUMN_POLAB(nband) (extfits_column){"POLAB", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_DEGREE, "", "Feed B orientation", (char*[]){""}}
///Feed B polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALB(npcal, nband) (extfits_column){"POLCALB", EXTFITS_ELEMENT_FLOAT.typestr itostr(npcal) "," itostr(nband), "", "", "Feed B polarization parameters", (char*[]){""}}
///degrees / m Antenna beam fwhm
#define FITSIDI_ANTENNA_COLUMN_BEAMFWHM(nband) (extfits_column){"BEAMFWHM", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_DEGREE, "", "/ m Antenna beam fwhm", (char*[]){""}}

///Polarization parameters
///Linear approximation for circular feeds
#define FITSIDI_ANTENNA_POLPARM_APPROX "APPROX"
///Linear approximation for linear feeds
#define FITSIDI_ANTENNA_POLPARM_LIN "X-Y LIN"
///Orientation and ellipticity
#define FITSIDI_ANTENNA_POLPARM_ORI_ELP "ORI-ELP"

///Mandatory keywords for the FITS-IDI ANTENNA table
///1
#define FITSIDI_ANTENNA_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///npcal = 0 or 2, number of polarization calibration constants
#define FITSIDI_ANTENNA_KEYWORD_NOPCAL (extfits_keyword){"NOPCAL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "npcal = 0 or 2, number of polarization calibration constants", (char*[]){""}}
///The feed polarization parameterization
#define FITSIDI_ANTENNA_KEYWORD_POLTYPE (extfits_keyword){"POLTYPE", EXTFITS_ELEMENT_STRING.typestr, "", "", "The feed polarization parameterization", (char*[]){""}}

///Columns for the FITS-IDI FREQUENCY table
///Frequency setup number
#define FITSIDI_FREQUENCY_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Hz Frequency offsets
#define FITSIDI_FREQUENCY_COLUMN_BANDFREQ(nband) (extfits_column){"BANDFREQ", EXTFITS_ELEMENT_DOUBLE.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Frequency offsets", (char*[]){""}}
///Hz Individual channel widths
#define FITSIDI_FREQUENCY_COLUMN_CH_WIDTH(nband) (extfits_column){"CH_WIDTH", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Individual channel widths", (char*[]){""}}
///Hz Total bandwidths of bands
#define FITSIDI_FREQUENCY_COLUMN_TOTAL_BANDWIDTH(nband) (extfits_column){"TOTAL_BANDWIDTH", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Total bandwidths of bands", (char*[]){""}}
///Sideband flag
#define FITSIDI_FREQUENCY_COLUMN_SIDEBAND(nband) (extfits_column){"SIDEBAND", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Sideband flag", (char*[]){""}}

///Mandatory keywords for the FITS-IDI FREQUENCY table
///1
#define FITSIDI_FREQUENCY_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}

///Frames of reference for VELTYP
///Local standard of rest
#define FITSIDI_SOURCE_VELTYP_LSR "LSR"
///Solar system barycenter
#define FITSIDI_SOURCE_VELTYP_BARYCENT "BARYCENT"
///Center of mass of the Earth
#define FITSIDI_SOURCE_VELTYP_GEOCENTR "GEOCENTR"
///Uncorrected
#define FITSIDI_SOURCE_VELTYP_TOPOCENT "TOPOCENT"

///Columns for the FITS-IDI SOURCE table
///Source ID number
#define FITSIDI_SOURCE_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Source name
#define FITSIDI_SOURCE_COLUMN_SOURCE (extfits_column){"SOURCE", "16A", "", "", "Source name", (char*[]){""}}
///Source name numeric qualifier
#define FITSIDI_SOURCE_COLUMN_QUAL (extfits_column){"QUAL", "1J", "", "", "Source name numeric qualifier", (char*[]){""}}
///Calibrator code
#define FITSIDI_SOURCE_COLUMN_CALCODE (extfits_column){"CALCODE", "4A", "", "", "Calibrator code", (char*[]){""}}
///Frequency setup number
#define FITSIDI_SOURCE_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Jy Stokes I flux density
#define FITSIDI_SOURCE_COLUMN_IFLUX(nband) (extfits_column){"IFLUX", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Jy", "", "Stokes I flux density", (char*[]){""}}
///Jy Stokes Q flux density
#define FITSIDI_SOURCE_COLUMN_QFLUX(nband) (extfits_column){"QFLUX", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Jy", "", "Stokes Q flux density", (char*[]){""}}
///Jy Stokes U flux density
#define FITSIDI_SOURCE_COLUMN_UFLUX(nband) (extfits_column){"UFLUX", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Jy", "", "Stokes U flux density", (char*[]){""}}
///Jy Stokes V flux density
#define FITSIDI_SOURCE_COLUMN_VFLUX(nband) (extfits_column){"VFLUX", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Jy", "", "Stokes V flux density", (char*[]){""}}
///Jy Spectral index for each band
#define FITSIDI_SOURCE_COLUMN_ALPHA(nband) (extfits_column){"ALPHA", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Jy", "", "Spectral index for each band", (char*[]){""}}
///Hz Frequency offset for each band
#define FITSIDI_SOURCE_COLUMN_FREQOFF(nband) (extfits_column){"FREQOFF", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Frequency offset for each band", (char*[]){""}}
///degrees Right ascension at mean equinox
#define FITSIDI_SOURCE_COLUMN_RAEPO (extfits_column){"RAEPO", "1D", EXTFITS_MEASURE_UNIT_DEGREE, "", "Right ascension at mean equinox", (char*[]){""}}
///degrees Declination at mean equinox
#define FITSIDI_SOURCE_COLUMN_DECEPO (extfits_column){"DECEPO", "1D", EXTFITS_MEASURE_UNIT_DEGREE, "", "Declination at mean equinox", (char*[]){""}}
///Mean equinox
#define FITSIDI_SOURCE_COLUMN_EQUINOX (extfits_column){"EQUINOX", "8A", "", "", "Mean equinox", (char*[]){""}}
///degrees Apparent right ascension
#define FITSIDI_SOURCE_COLUMN_RAAPP (extfits_column){"RAAPP", "1D", EXTFITS_MEASURE_UNIT_DEGREE, "", "Apparent right ascension", (char*[]){""}}
///degrees Apparent declination
#define FITSIDI_SOURCE_COLUMN_DECAPP (extfits_column){"DECAPP", "1D", EXTFITS_MEASURE_UNIT_DEGREE, "", "Apparent declination", (char*[]){""}}
///meters/sec Systemic velocity for each band
#define FITSIDI_SOURCE_COLUMN_SYSVEL(nband) (extfits_column){"SYSVEL", EXTFITS_ELEMENT_DOUBLE.typestr itostr(nband), "meters/sec", "", "Systemic velocity for each band", (char*[]){""}}
///Velocity type
#define FITSIDI_SOURCE_COLUMN_VELTYP (extfits_column){"VELTYP", "8A", "", "", "Velocity type", (char*[]){""}}
///Velocity definition
#define FITSIDI_SOURCE_COLUMN_VELDEF (extfits_column){"VELDEF", "8A", "", "", "Velocity definition", (char*[]){""}}
///Hz Line rest frequency for each band
#define FITSIDI_SOURCE_COLUMN_RESTFREQ(nband) (extfits_column){"RESTFREQ", EXTFITS_ELEMENT_DOUBLE.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Line rest frequency for each band", (char*[]){""}}
///degrees/day Proper motion in right ascension
#define FITSIDI_SOURCE_COLUMN_PMRA (extfits_column){"PMRA", "1D", "degrees/day", "", "Proper motion in right ascension", (char*[]){""}}
///degrees/day Proper motion in declination
#define FITSIDI_SOURCE_COLUMN_PMDEC (extfits_column){"PMDEC", "1D", "degrees/day", "", "Proper motion in declination", (char*[]){""}}
///arcseconds Parallax of source
#define FITSIDI_SOURCE_COLUMN_PARALLAX (extfits_column){"PARALLAX", "1E", EXTFITS_MEASURE_UNIT_ARCSEC, "", "Parallax of source", (char*[]){""}}
///years Epoch of observation
#define FITSIDI_SOURCE_COLUMN_EPOCH (extfits_column){"EPOCH", "1D", EXTFITS_MEASURE_UNIT_YEAR, "", "Epoch of observation", (char*[]){""}}

///Mandatory keywords for the FITS-IDI SOURCE table
///1
#define FITSIDI_SOURCE_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}

///Columns for the FITS-IDI INTERFEROMETER_MODEL table
///days Starting time of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Starting time of interval", (char*[]){""}}
///days Duration of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Antenna number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///rad m−2 Ionospheric Faraday rotation
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_I_FAR_ROT (extfits_column){"I.FAR.ROT", "1E", "rad m−2", "", "Ionospheric Faraday rotation", (char*[]){""}}
///Hz Time variable frequency offsets
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQ_VAR(nband) (extfits_column){"FREQ.VAR", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Time variable frequency offsets", (char*[]){""}}
///turns Phase delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_1(npoly, nband) (extfits_column){"PDELAY_1", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), "turns", "", "Phase delay polynomials for polarization 1", (char*[]){""}}
///seconds Group delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_1(npoly, nband) (extfits_column){"GDELAY_1", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), EXTFITS_MEASURE_UNIT_SECOND, "", "Group delay polynomials for polarization 1", (char*[]){""}}
///Hz Phase delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_1(npoly, nband) (extfits_column){"PRATE_1", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Phase delay rate polynomials for polarization 1", (char*[]){""}}
///sec/sec Group delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_1(npoly, nband) (extfits_column){"GRATE_1", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), "sec/sec", "", "Group delay rate polynomials for polarization 1", (char*[]){""}}
///sec m−2 Dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_1 (extfits_column){"DISP_1", "1E", EXTFITS_MEASURE_UNIT_SECOND, "", "Dispersive delay for polarization 1", (char*[]){""}}
///sec m−2/sec Rate of change of dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_1 (extfits_column){"DDISP_1", "1E", "sec m−2/sec", "", " Rate of change of dispersive delay for polarization 1", (char*[]){""}}
///turns Phase delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_2(npoly, nband) (extfits_column){"PDELAY_2", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), "turns", "", "Phase delay polynomials for polarization 2", (char*[]){""}}
///seconds Group delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_2(npoly, nband) (extfits_column){"GDELAY_2", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), EXTFITS_MEASURE_UNIT_SECOND, "", "Group delay polynomials for polarization 2", (char*[]){""}}
///Hz Phase delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_2(npoly, nband) (extfits_column){"PRATE_2", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Phase delay rate polynomials for polarization 2", (char*[]){""}}
///sec/sec Group delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_2(npoly, nband) (extfits_column){"GRATE_2", EXTFITS_ELEMENT_DOUBLE.typestr itostr(npoly) "," itostr(nband), "sec/sec", "", "Group delay rate polynomials for polarization 2", (char*[]){""}}
///sec m−2 Dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_2 (extfits_column){"DISP_2", "1E", EXTFITS_MEASURE_UNIT_SECOND, "", "Dispersive delay for polarization 2", (char*[]){""}}
///sec m−2/sec Rate of change of dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_2 (extfits_column){"DDISP_2", "1E", "sec m−2/sec", "", " Rate of change of dispersive delay for polarization 2", (char*[]){""}}

///Mandatory keywords for the FITS-IDI INTERFEROMETER_MODEL table
///2
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "2", (char*[]){""}}
///Number of polynomial terms npoly
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NPOLY (extfits_keyword){"NPOLY", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polynomial terms npoly", (char*[]){""}}
///Number of polarizations
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations", (char*[]){""}}

///Columns for the FITS-IDI SYSTEM_TEMPERATURE table
///days Central time of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///days Duration of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Antenna number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Kelvin System temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_1(nband) (extfits_column){"TSYS_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "System temperatures for polarization 1", (char*[]){""}}
///Kelvin Antenna temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_1(nband) (extfits_column){"TANT_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "Antenna temperatures for polarization 1", (char*[]){""}}
///Kelvin System temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_2(nband) (extfits_column){"TSYS_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "System temperatures for polarization 2", (char*[]){""}}
///Kelvin Antenna temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_2(nband) (extfits_column){"TANT_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "Antenna temperatures for polarization 2", (char*[]){""}}

///Mandatory keywords for the FITS-IDI SYSTEM_TEMPERATURE table
///1
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Number of polarizations in the table
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations in the table", (char*[]){""}}

///Types for x and y values
///None
#define XY_None "0"
///Elevation in degrees
#define XY_Elevation_in_degrees "1"
///Zenith angle in degrees
#define XY_Zenith_angle_in_degrees "2"
///Hour angle in degrees
#define XY_Hour_angle_in_degrees "3"
///Declination in degrees
#define XY_Declination_in_degrees "4"
///Co-declination in degrees
#define XY_Codeclination_in_degrees "5"

///Spherical harmonic coefficients in GAIN_1 and GAIN 2
///A00
#define spherical_harmonic_coefficients_A00 "1"
///A10
#define spherical_harmonic_coefficients_A10 "2"
///A11E
#define spherical_harmonic_coefficients_A11E "3"
///A110
#define spherical_harmonic_coefficients_A110 "4"
///A20
#define spherical_harmonic_coefficients_A20 "5"
///A21E
#define spherical_harmonic_coefficients_A21E "6"
///A210
#define spherical_harmonic_coefficients_A210 "7"
///A22E
#define spherical_harmonic_coefficients_A22E "8"
///A220
#define spherical_harmonic_coefficients_A220 "9"
///A30
#define spherical_harmonic_coefficients_A30 "10"

///Columns for the FITS-IDI GAIN_CURVE table
///Antenna number
#define FITSIDI_GAIN_CURVE_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_GAIN_CURVE_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_GAIN_CURVE_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Gain curve types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_1(nband) (extfits_column){"TYPE_1", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Gain curve types for polarization 1", (char*[]){""}}
///Number of terms or entries for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_1(nband) (extfits_column){"NTERM_1", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Number of terms or entries for polarization 1", (char*[]){""}}
///x value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_1(nband) (extfits_column){"X_TYP_1", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "x value types for polarization 1", (char*[]){""}}
///y value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_1(nband) (extfits_column){"Y_TYP_1", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "y value types for polarization 1", (char*[]){""}}
///x values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_1(nband) (extfits_column){"X_VAL_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "x values for polarization 1", (char*[]){""}}
///y values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_1(ntab, nband) (extfits_column){"Y_VAL_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntab) "," itostr(nband), "", "", "y values for polarization 1", (char*[]){""}}
///Relative gain values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_1(ntab, nband) (extfits_column){"GAIN_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntab) "," itostr(nband), "", "", "Relative gain values for polarization 1", (char*[]){""}}
///K/Jy Sensitivities for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_1(nband) (extfits_column){"SENS_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "K/Jy", "", " Sensitivities for polarization 1", (char*[]){""}}
///Gain curve types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_2(nband) (extfits_column){"TYPE_2", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Gain curve types for polarization 2", (char*[]){""}}
///Number of terms or entries for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_2(nband) (extfits_column){"NTERM_2", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Number of terms or entries for polarization 2", (char*[]){""}}
///x value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_2(nband) (extfits_column){"X_TYP_2", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "x value types for polarization 2", (char*[]){""}}
///y value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_2(nband) (extfits_column){"Y_TYP_2", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "y value types for polarization 2", (char*[]){""}}
///x values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_2(nband) (extfits_column){"X_VAL_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "x values for polarization 2", (char*[]){""}}
///y values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_2(ntab, nband) (extfits_column){"Y_VAL_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntab) "," itostr(nband), "", "", "y values for polarization 2", (char*[]){""}}
///Relative gain values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_2(ntab, nband) (extfits_column){"GAIN_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntab) "," itostr(nband), "", "", "Relative gain values for polarization 2", (char*[]){""}}
///K/Jy Sensitivities for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_2(nband) (extfits_column){"SENS_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "K/Jy", "", " Sensitivities for polarization 2", (char*[]){""}}

///Mandatory keywords for the FITS-IDI GAIN_CURVE table
///1
#define FITSIDI_GAIN_CURVE_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Number of polarizations in the table
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations in the table", (char*[]){""}}
///Number of tabulated values ntab
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_TABS (extfits_keyword){"NO_TABS", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of tabulated values ntab", (char*[]){""}}

///Columns for the FITS-IDI PHASE-CAL table
///days Central time of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///days Duration of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_PHASE_CAL_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Antenna number
#define FITSIDI_PHASE_CAL_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_PHASE_CAL_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_PHASE_CAL_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///seconds Cable calibration measurement
#define FITSIDI_PHASE_CAL_COLUMN_CABLE_CAL (extfits_column){"CABLE_CAL", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "Cable calibration measurement", (char*[]){""}}
///percent State counts for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_STATE_1(nband) (extfits_column){"STATE_1", "E4,", nband), EXTFITS_MEASURE_UNIT_PERCENT, "", "State counts for polarization 1", (char*[]){""}}
///Hz Phase-cal tone frequencies for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_1(ntone, nband) (extfits_column){"PC_FREQ_1", EXTFITS_ELEMENT_DOUBLE.typestr itostr(ntone) "," itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Phase-cal tone frequencies for polarization 1", (char*[]){""}}
///Real parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_1(ntone, nband) (extfits_column){"PC_REAL_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntone) "," itostr(nband), "", "", "Real parts of phase-cal measurements for polarization 1", (char*[]){""}}
///Imaginary parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_1(ntone, nband) (extfits_column){"PC_IMAG_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntone) "," itostr(nband), "", "", "Imaginary parts of phase-cal measurements for polarization 1", (char*[]){""}}
///sec/sec Phase-cal rates for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_1(ntone, nband) (extfits_column){"PC_RATE_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntone) "," itostr(nband), "sec/sec", "", "Phase-cal rates for polarization 1", (char*[]){""}}
///percent State counts for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_STATE_2(nband) (extfits_column){"STATE_2", "E4,", nband), EXTFITS_MEASURE_UNIT_PERCENT, "", "State counts for polarization 2", (char*[]){""}}
///Hz Phase-cal tone frequencies for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_2(ntone, nband) (extfits_column){"PC_FREQ_2", EXTFITS_ELEMENT_DOUBLE.typestr itostr(ntone) "," itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Phase-cal tone frequencies for polarization 2", (char*[]){""}}
///Real parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_2(ntone, nband) (extfits_column){"PC_REAL_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntone) "," itostr(nband), "", "", "Real parts of phase-cal measurements for polarization 2", (char*[]){""}}
///Imaginary parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_2(ntone, nband) (extfits_column){"PC_IMAG_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntone) "," itostr(nband), "", "", "Imaginary parts of phase-cal measurements for polarization 2", (char*[]){""}}
///sec/sec Phase-cal rates for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_2(ntone, nband) (extfits_column){"PC_RATE_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(ntone) "," itostr(nband), "sec/sec", "", "Phase-cal rates for polarization 2", (char*[]){""}}

///Mandatory keywords for the FITS-IDI PHASE-CAL table
///2
#define FITSIDI_PHASE_CAL_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "2", (char*[]){""}}
///Number of polarizations in the table
#define FITSIDI_PHASE_CAL_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations in the table", (char*[]){""}}
///Number of tones ntone
#define FITSIDI_PHASE_CAL_KEYWORD_NO_TABS (extfits_keyword){"NO_TABS", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of tones ntone", (char*[]){""}}

///Recommended SEVERITY codes
///No severity level assigned
#define severity_No_severity_level_assigned "-1"
///Data are known to be useless
#define severity_Data_are_known_to_be_useless "0"
///Data are probably useless
#define severity_Data_are_probably_useless "1"
///Data may be useless
#define severity_Data_may_be_useless "2"

///Columns for the FITS-IDI FLAG table
///Source ID number
#define FITSIDI_FLAG_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Array number
#define FITSIDI_FLAG_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Antenna numbers
#define FITSIDI_FLAG_COLUMN_ANTS (extfits_column){"ANTS", "2J", "", "", "Antenna numbers", (char*[]){""}}
///Frequency setup number
#define FITSIDI_FLAG_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///days Time range
#define FITSIDI_FLAG_COLUMN_TIMERANG (extfits_column){"TIMERANG", "2E", EXTFITS_MEASURE_UNIT_DAY, "", "Time range", (char*[]){""}}
///Band flags
#define FITSIDI_FLAG_COLUMN_BANDS(nband) (extfits_column){"BANDS", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Band flags", (char*[]){""}}
///Channel range
#define FITSIDI_FLAG_COLUMN_CHANS (extfits_column){"CHANS", "2J", "", "", "Channel range", (char*[]){""}}
///Polarization flags
#define FITSIDI_FLAG_COLUMN_PFLAGS (extfits_column){"PFLAGS", "4J", "", "", "Polarization flags", (char*[]){""}}
///Reason for flag
#define FITSIDI_FLAG_COLUMN_REASON(n) (extfits_column){"REASON" itostr(n), EXTFITS_ELEMENT_STRING.typestr, "", "", "Reason for flag", (char*[]){""}}
///Severity code
#define FITSIDI_FLAG_COLUMN_SEVERITY (extfits_column){"SEVERITY", "1J", "", "", "Severity code", (char*[]){""}}

///Mandatory keywords for the FITS-IDI FLAG table
///2
#define FITSIDI_FLAG_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "2", (char*[]){""}}

///Columns for the FITS-IDI WEATHER table
///days Central time of interval
#define FITSIDI_WEATHER_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///days Duration of interval
#define FITSIDI_WEATHER_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of interval", (char*[]){""}}
///Antenna number
#define FITSIDI_WEATHER_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Centigrade Surface air temperature
#define FITSIDI_WEATHER_COLUMN_TEMPERATURE (extfits_column){"TEMPERATURE", "1E", "", "", "Centigrade Surface air temperature", (char*[]){""}}
///millibar Surface air pressure
#define FITSIDI_WEATHER_COLUMN_PRESSURE (extfits_column){"PRESSURE", "1E", "millibar", "", "Surface air pressure", (char*[]){""}}
///Centigrade Dewpoint temperature
#define FITSIDI_WEATHER_COLUMN_DEWPOINT (extfits_column){"DEWPOINT", "1E", "", "", "Centigrade Dewpoint temperature", (char*[]){""}}
///m s−1 Wind velocity
#define FITSIDI_WEATHER_COLUMN_WIND_VELOCITY (extfits_column){"WIND_VELOCITY", "1E", "m s−1", "", " Wind velocity", (char*[]){""}}
///degrees Wind direction East from North
#define FITSIDI_WEATHER_COLUMN_WIND_DIRECTION (extfits_column){"WIND_DIRECTION", "1E", EXTFITS_MEASURE_UNIT_DEGREE, "", "Wind direction East from North", (char*[]){""}}
///m−2 Water column
#define FITSIDI_WEATHER_COLUMN_WVR_H2O (extfits_column){"WVR_H2O", "1E", "m−2", "", "Water column", (char*[]){""}}
///m−2 Electron column
#define FITSIDI_WEATHER_COLUMN_IONOS_ELECTRON (extfits_column){"IONOS_ELECTRON", "1E", "m−2", "", "Electron column", (char*[]){""}}

///Mandatory keywords for the FITS-IDI WEATHER table
///2
#define FITSIDI_WEATHER_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "2", (char*[]){""}}
///Reference date
#define FITSIDI_WEATHER_KEYWORD_RDATE (extfits_keyword){"RDATE", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Reference date", (char*[]){""}}

///Columns for the FITS-IDI BASELINE table
///days Central time of interval
#define FITSIDI_BASELINE_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_BASELINE_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Array number
#define FITSIDI_BASELINE_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Antenna numbers forming baseline
#define FITSIDI_BASELINE_COLUMN_ANTENNA_NOS (extfits_column){"ANTENNA_NOS.", "2J", "", "", "Antenna numbers forming baseline", (char*[]){""}}
///Frequency setup number
#define FITSIDI_BASELINE_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Real part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_REAL_M(nstokes, nband) (extfits_column){"REAL_M", EXTFITS_ELEMENT_FLOAT.typestr itostr(nstokes) "," itostr(nband), "", "", "Real part of multiplicative correction", (char*[]){""}}
///Imaginary part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_IMAG_M(nstokes, nband) (extfits_column){"IMAG_M", EXTFITS_ELEMENT_FLOAT.typestr itostr(nstokes) "," itostr(nband), "", "", "Imaginary part of multiplicative correction", (char*[]){""}}
///Real part of additive correction
#define FITSIDI_BASELINE_COLUMN_REAL_A(nstokes, nband) (extfits_column){"REAL_A", EXTFITS_ELEMENT_FLOAT.typestr itostr(nstokes) "," itostr(nband), "", "", "Real part of additive correction", (char*[]){""}}
///Imaginary part of additive correction
#define FITSIDI_BASELINE_COLUMN_IMAG_A(nstokes, nband) (extfits_column){"IMAG_A", EXTFITS_ELEMENT_FLOAT.typestr itostr(nstokes) "," itostr(nband), "", "", "Imaginary part of additive correction", (char*[]){""}}

///Mandatory keywords for the FITS-IDI BASELINE table
///1
#define FITSIDI_BASELINE_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Maximum antenna number in the table
#define FITSIDI_BASELINE_KEYWORD_NO_ANT (extfits_keyword){"NO_ANT", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Maximum antenna number in the table", (char*[]){""}}

///Columns for the FITS-IDI BANDPASS table
///days Central time of interval
#define FITSIDI_BANDPASS_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///days Duration of interval
#define FITSIDI_BANDPASS_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_BANDPASS_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Antenna number
#define FITSIDI_BANDPASS_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_BANDPASS_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_BANDPASS_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Hz Channel bandwidth
#define FITSIDI_BANDPASS_COLUMN_BANDWIDTH (extfits_column){"BANDWIDTH", "1E", EXTFITS_MEASURE_UNIT_HZ, "", "Channel bandwidth", (char*[]){""}}
///Hz Frequency of each band
#define FITSIDI_BANDPASS_COLUMN_BAND_FREQ(nband) (extfits_column){"BAND_FREQ", EXTFITS_ELEMENT_DOUBLE.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "Frequency of each band", (char*[]){""}}
///Reference antenna for polarization 1
#define FITSIDI_BANDPASS_COLUMN_REFANT_1 (extfits_column){"REFANT_1", "1J", "", "", "Reference antenna for polarization 1", (char*[]){""}}
///Real part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BREAL_1(nbach, nband) (extfits_column){"BREAL_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nbach) "," itostr(nband), "", "", "Real part of bandpass correction for polarization 1", (char*[]){""}}
///Imaginary part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BIMAG_1(nbach, nband) (extfits_column){"BIMAG_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nbach) "," itostr(nband), "", "", "Imaginary part of bandpass correction for polarization 1", (char*[]){""}}
///Reference antenna for polarization 2
#define FITSIDI_BANDPASS_COLUMN_REFANT_2 (extfits_column){"REFANT_2", "1J", "", "", "Reference antenna for polarization 2", (char*[]){""}}
///Real part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BREAL_2(nbach, nband) (extfits_column){"BREAL_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nbach) "," itostr(nband), "", "", "Real part of bandpass correction for polarization 2", (char*[]){""}}
///Imaginary part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BIMAG_2(nbach, nband) (extfits_column){"BIMAG_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nbach) "," itostr(nband), "", "", "Imaginary part of bandpass correction for polarization 2", (char*[]){""}}

///Mandatory keywords for the FITS-IDI BANDPASS table
///1
#define FITSIDI_BANDPASS_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Maximum antenna number in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_ANT (extfits_keyword){"NO_ANT", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Maximum antenna number in the table", (char*[]){""}}
///Number of polarizations in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations in the table", (char*[]){""}}
///Number of spectral channels in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_BACH (extfits_keyword){"NO_BACH", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of spectral channels in the table", (char*[]){""}}
///Data channel number for first channel in the table
#define FITSIDI_BANDPASS_KEYWORD_STRT_CHN (extfits_keyword){"STRT_CHN", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Data channel number for first channel in the table", (char*[]){""}}

///Columns for the FITS-IDI CALIBRATION table
///days Central time of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///days Duration of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME_INTERVAL (extfits_column){"TIME_INTERVAL", "1E", EXTFITS_MEASURE_UNIT_DAY, "", "Duration of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_CALIBRATION_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Antenna number
#define FITSIDI_CALIBRATION_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_CALIBRATION_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_CALIBRATION_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///Kelvin System temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TSYS_1(nband) (extfits_column){"TSYS_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "System temperature for polarization 1", (char*[]){""}}
///Kelvin Antenna temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TANT_1(nband) (extfits_column){"TANT_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "Antenna temperature for polarization 1", (char*[]){""}}
///Kelvin/Jy Sensitivity at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_1(nband) (extfits_column){"SENSITIVITY_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Kelvin/Jy", "", "Sensitivity at polarization 1", (char*[]){""}}
///radians Phase at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_PHASE_1(nband) (extfits_column){"PHASE_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_RAD, "", "Phase at polarization 1", (char*[]){""}}
///sec/sec Rate of change of delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_RATE_1(nband) (extfits_column){"RATE_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "sec/sec", "", "Rate of change of delay of polarization 1", (char*[]){""}}
///seconds Delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_DELAY_1(nband) (extfits_column){"DELAY_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_SECOND, "", "Delay of polarization 1", (char*[]){""}}
///Complex gain real part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REAL_1(nband) (extfits_column){"REAL_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "Complex gain real part for polarization 1", (char*[]){""}}
///Complex gain imaginary part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_IMAG_1(nband) (extfits_column){"IMAG_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "Complex gain imaginary part for polarization 1", (char*[]){""}}
///Reliability weight of complex gain for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_1(nband) (extfits_column){"WEIGHT_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "Reliability weight of complex gain for polarization 1", (char*[]){""}}
///Reference antenna for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REFANT_1(nband) (extfits_column){"REFANT_1", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Reference antenna for polarization 1", (char*[]){""}}
///Kelvin System temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TSYS_2(nband) (extfits_column){"TSYS_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "System temperature for polarization 2", (char*[]){""}}
///Kelvin Antenna temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TANT_2(nband) (extfits_column){"TANT_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_KELVIN, "", "Antenna temperature for polarization 2", (char*[]){""}}
///Kelvin/Jy Sensitivity at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_2(nband) (extfits_column){"SENSITIVITY_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Kelvin/Jy", "", "Sensitivity at polarization 2", (char*[]){""}}
///radians Phase at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_PHASE_2(nband) (extfits_column){"PHASE_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_RAD, "", "Phase at polarization 2", (char*[]){""}}
///sec/sec Rate of change of delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_RATE_2(nband) (extfits_column){"RATE_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "sec/sec", "", "Rate of change of delay of polarization 2", (char*[]){""}}
///seconds Delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_DELAY_2(nband) (extfits_column){"DELAY_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_SECOND, "", "Delay of polarization 2", (char*[]){""}}
///Complex gain real part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REAL_2(nband) (extfits_column){"REAL_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "Complex gain real part for polarization 2", (char*[]){""}}
///Complex gain imaginary part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_IMAG_2(nband) (extfits_column){"IMAG_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "Complex gain imaginary part for polarization 2", (char*[]){""}}
///Reliability weight of complex gain for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_2(nband) (extfits_column){"WEIGHT_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "", "", "Reliability weight of complex gain for polarization 2", (char*[]){""}}
///Reference antenna for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REFANT_2(nband) (extfits_column){"REFANT_2", EXTFITS_ELEMENT_INT.typestr itostr(nband), "", "", "Reference antenna for polarization 2", (char*[]){""}}

///Mandatory keywords for the FITS-IDI CALIBRATION table
///1
#define FITSIDI_CALIBRATION_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Maximum antenna number in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_ANT (extfits_keyword){"NO_ANT", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Maximum antenna number in the table", (char*[]){""}}
///Number of polarizations in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations in the table", (char*[]){""}}

///Columns for the FITS-IDI MODEL_COMPS table
///days Central time of interval
#define FITSIDI_MODEL_COMPS_COLUMN_TIME (extfits_column){"TIME", "1D", EXTFITS_MEASURE_UNIT_DAY, "", "Central time of interval", (char*[]){""}}
///Source ID number
#define FITSIDI_MODEL_COMPS_COLUMN_SOURCE_ID (extfits_column){"SOURCE_ID", "1J", "", "", "Source ID number", (char*[]){""}}
///Antenna number
#define FITSIDI_MODEL_COMPS_COLUMN_ANTENNA_NO (extfits_column){"ANTENNA_NO", "1J", "", "", "Antenna number", (char*[]){""}}
///Array number
#define FITSIDI_MODEL_COMPS_COLUMN_ARRAY (extfits_column){"ARRAY", "1J", "", "", "Array number", (char*[]){""}}
///Frequency setup number
#define FITSIDI_MODEL_COMPS_COLUMN_FREQID (extfits_column){"FREQID", "1J", "", "", "Frequency setup number", (char*[]){""}}
///sec Atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_ATMOS (extfits_column){"ATMOS", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "Atmospheric delay", (char*[]){""}}
///sec/sec Time derivative of atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_DATMOS (extfits_column){"DATMOS", "1D", "sec/sec", "", "Time derivative of atmospheric delay", (char*[]){""}}
///sec Group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GDELAY (extfits_column){"GDELAY", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "Group delay", (char*[]){""}}
///sec/sec Rate of change of group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GRATE (extfits_column){"GRATE", "1D", "sec/sec", "", "Rate of change of group delay", (char*[]){""}}
///sec 'Clock' epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_1 (extfits_column){"CLOCK_1", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "'Clock' epoch error", (char*[]){""}}
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_1 (extfits_column){"DCLOCK_1", "1D", "sec/sec", "", "Time derivative of clock error", (char*[]){""}}
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_1(nband) (extfits_column){"LO_OFFSET_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "LO offset", (char*[]){""}}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_1(nband) (extfits_column){"DLO_OFFSET_1", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Hz/sec", "", "Time derivative of LO offset", (char*[]){""}}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_1 (extfits_column){"DISP_1", "1E", EXTFITS_MEASURE_UNIT_SECOND, "", "Dispersive delay", (char*[]){""}}
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_1 (extfits_column){"DDISP_1", "1E", "sec m−2/sec", "", " Time derivative of dispersive delay", (char*[]){""}}
///sec 'Clock' epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_2 (extfits_column){"CLOCK_2", "1D", EXTFITS_MEASURE_UNIT_SECOND, "", "'Clock' epoch error", (char*[]){""}}
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_2 (extfits_column){"DCLOCK_2", "1D", "sec/sec", "", "Time derivative of clock error", (char*[]){""}}
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_2(nband) (extfits_column){"LO_OFFSET_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), EXTFITS_MEASURE_UNIT_HZ, "", "LO offset", (char*[]){""}}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_2(nband) (extfits_column){"DLO_OFFSET_2", EXTFITS_ELEMENT_FLOAT.typestr itostr(nband), "Hz/sec", "", "Time derivative of LO offset", (char*[]){""}}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_2 (extfits_column){"DISP_2", "1E", EXTFITS_MEASURE_UNIT_SECOND, "", "Dispersive delay", (char*[]){""}}
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_2 (extfits_column){"DDISP_2", "1E", "sec m−2/sec", "", "Time derivative of dispersive delay", (char*[]){""}}

///Mandatory keywords for the FITS-IDI MODEL_COMPS table
///1
#define FITSIDI_MODEL_COMPS_KEYWORD_TABREV (extfits_keyword){"TABREV", EXTFITS_ELEMENT_SHORT.typestr, "", "", "1", (char*[]){""}}
///Reference date
#define FITSIDI_MODEL_COMPS_KEYWORD_RDATE (extfits_keyword){"RDATE", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Reference date", (char*[]){""}}
///Number of polarizations in the table
#define FITSIDI_MODEL_COMPS_KEYWORD_NO_POL (extfits_keyword){"NO_POL", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Number of polarizations in the table", (char*[]){""}}
///FFT size
#define FITSIDI_MODEL_COMPS_KEYWORD_FFT_SIZE (extfits_keyword){"FFT_SIZE", EXTFITS_ELEMENT_SHORT.typestr, "", "", "FFT size", (char*[]){""}}
///Oversampling factor
#define FITSIDI_MODEL_COMPS_KEYWORD_OVERSAMP (extfits_keyword){"OVERSAMP", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Oversampling factor", (char*[]){""}}
///Zero padding factor
#define FITSIDI_MODEL_COMPS_KEYWORD_ZERO_PAD (extfits_keyword){"ZERO_PAD", EXTFITS_ELEMENT_SHORT.typestr, "", "", "Zero padding factor", (char*[]){""}}
///Tapering function ('HANNING' or 'UNIFORM')
#define FITSIDI_MODEL_COMPS_KEYWORD_TAPER_FN (extfits_keyword){"TAPER_FN", EXTFITS_ELEMENT_STRING.typestr, "", "", "Tapering function ('HANNING' or 'UNIFORM')", (char*[]){""}}

///SDFITS Convention Table
#define FITS_TABLE_SDFITS "SINGLE DISH"

///SDFITS columns
///common FITS usage
#define SDFITS_COLUMN_OBJECT (extfits_column){"OBJECT", "8A", "", "", "common FITS usage", (char*[]){""}}
///common FITS keyword
#define SDFITS_COLUMN_TELESCOP (extfits_column){"TELESCOP", "8A", "", "", "common FITS keyword", (char*[]){""}}
///resolution may differ from spacing of backend, not one channel
#define SDFITS_COLUMN_FREQRES (extfits_column){"FREQRES", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_HZ, "", "resolution may differ from spacing of backend, not one channel", (char*[]){""}}
#define SDFITS_COLUMN_BANDWID (extfits_column){"BANDWID", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_HZ, "", "", (char*[]){""}}
///common FITS usage; JD preferable?
#define SDFITS_COLUMN_DATE_OBS (extfits_column){"DATE-OBS", "8A", "", "", "common FITS usage; JD preferable?", (char*[]){""}}
///UT time of day; UT seconds since Oh UT
#define SDFITS_COLUMN_TIME (extfits_column){"TIME", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_SECOND, "", "UT time of day; UT seconds since Oh UT", (char*[]){""}}
///effective integration time
#define SDFITS_COLUMN_EXPOSURE (extfits_column){"EXPOSURE", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_SECOND, "", "effective integration time", (char*[]){""}}
///system, not receiver, temperature
#define SDFITS_COLUMN_TSYS (extfits_column){"TSYS", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "system, not receiver, temperature", (char*[]){""}}
///Target right ascension coordinate
#define SDFITS_COLUMN_OBJCTRA (extfits_column){"OBJCTRA", EXTFITS_ELEMENT_STRING.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Target right ascension coordinate", (char*[]){""}}
///Target declination coordinate
#define SDFITS_COLUMN_OBJCTDEC (extfits_column){"OBJCTDEC", EXTFITS_ELEMENT_STRING.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Target declination coordinate", (char*[]){""}}
///Data buffer
#define SDFITS_COLUMN_DATA (extfits_column){"DATA", "", "", "", "", (char*[]){""}}

///OBSMODE
///average
#define OBSTYPE_LINE "LINE"
///continuum
#define OBSTYPE_CONT "CONT"
///pulses
#define OBSTYPE_PULS "PULS"

///position switch
#define OBSMODE_PSSW "PSSW"
///frequency switch
#define OBSMODE_FQSW "FQSW"
///beam switch
#define OBSMODE_BMSW "BMSW"
///phase-lock switch
#define OBSMODE_PLSW "PLSW"
///load switch
#define OBSMODE_LDSW "LDSW"
///total power
#define OBSMODE_TLPW "TLPW"

///average
#define OBSMODE_LINE_PSSW OBSTYPE_LINE OBSMODE_PSSW
///frequency switch
#define OBSMODE_LINE_FQSW OBSTYPE_LINE OBSMODE_FQSW
///beam switch
#define OBSMODE_LINE_BMSW OBSTYPE_LINE OBSMODE_BMSW
///phase-lock switch
#define OBSMODE_LINE_PLSW OBSTYPE_LINE OBSMODE_PLSW
///load switch
#define OBSMODE_LINE_LDSW OBSTYPE_LINE OBSMODE_LDSW
///total power
#define OBSMODE_LINE_TLPW OBSTYPE_LINE OBSMODE_TLPW

///continuum
///position switch
#define OBSMODE_CONT_PSSW OBSTYPE_CONT OBSMODE_PSSW
///frequency switch
#define OBSMODE_CONT_FQSW OBSTYPE_CONT OBSMODE_FQSW
///beam switch
#define OBSMODE_CONT_BMSW OBSTYPE_CONT OBSMODE_BMSW
///phase-lock switch
#define OBSMODE_CONT_PLSW OBSTYPE_CONT OBSMODE_PLSW
///load switch
#define OBSMODE_CONT_LDSW OBSTYPE_CONT OBSMODE_LDSW
///total power
#define OBSMODE_CONT_TLPW OBSTYPE_CONT OBSMODE_TLPW

///pulses
///position switch
#define OBSMODE_PULS_PSSW OBSTYPE_PULS OBSMODE_PSSW
///frequency switch
#define OBSMODE_PULS_FQSW OBSTYPE_PULS OBSMODE_FQSW
///beam switch
#define OBSMODE_PULS_BMSW OBSTYPE_PULS OBSMODE_BMSW
///phase-lock switch
#define OBSMODE_PULS_PLSW OBSTYPE_PULS OBSMODE_PLSW
///load switch
#define OBSMODE_PULS_LDSW OBSTYPE_PULS OBSMODE_LDSW
///total power
#define OBSMODE_PULS_TLPW OBSTYPE_PULS OBSMODE_TLPW

///TEMPSCAL
#define TEMPSCAL_TB "TB"
#define TEMPSCAL_TA "TA"
#define TEMPSCAL_TA_TR "TA*TR"
#define TEMPSCAL_TR "TR*"

///VELDEF
#define VELDEF_RADI "*RADI"
#define VELDEF_OPTI "OPTI"
#define VELDEF_RELA "RELA"
#define VELDEF_LSR "LSR"
#define VELDEF_HELO "HELO"
#define VELDEF_EART "EART"
#define VELDEF_BARI "BARI"
#define VELDEF_OBS "-OBS"

///SDFITS columns
///Observer name
#define SDFITS_COLUMN_OBSERVER (extfits_column){"OBSERVER", "8A", "", "", "Observer name", (char*[]){""}}
///Observer & operator initials;
#define SDFITS_COLUMN_OBSID (extfits_column){"OBSID", "8A", "", "", "Observer & operator initials", (char*[]){""}}
///Project ID;
#define SDFITS_COLUMN_PROJID (extfits_column){"PROJID", "8A", "", "", "Project ID", (char*[]){""}}
///Scan number
#define SDFITS_COLUMN_SCAN (extfits_column){"SCAN", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Scan number", (char*[]){""}}
///Type of data, observing mode;
#define SDFITS_COLUMN_OBSMODE (extfits_column){"OBSMODE", "8A", "", "", "Type of data, observing mode", (char*[]){OBSMODE_LINE_PSSW, OBSMODE_LINE_FQSW, OBSMODE_LINE_BMSW, OBSMODE_LINE_PLSW, OBSMODE_LINE_LDSW, OBSMODE_LINE_TLPW, OBSMODE_CONT_PSSW, OBSMODE_CONT_FQSW, OBSMODE_CONT_BMSW, OBSMODE_CONT_PLSW, OBSMODE_CONT_LDSW, OBSMODE_CONT_TLPW, OBSMODE_PULS_PSSW, OBSMODE_PULS_FQSW, OBSMODE_PULS_BMSW, OBSMODE_PULS_PLSW, OBSMODE_PULS_LDSW, OBSMODE_PULS_TLPW}}
///Molecule observed or detected;
#define SDFITS_COLUMN_MOLECULE (extfits_column){"MOLECULE", "8A", "", "", "Molecule observed or detected", (char*[]){""}}
///As appropriate;
#define SDFITS_COLUMN_TRANSITI (extfits_column){"TRANSITI", "8A", "", "", "As appropriate", (char*[]){""}}
///Normalization of TA;
#define SDFITS_COLUMN_TEMPSCAL (extfits_column){"TEMPSCAL", "8A", "", "", "Normalization of TA", (char*[]){""}}
///
#define SDFITS_COLUMN_FRONTEND (extfits_column){"FRONTEND", "8A", "", "", "", (char*[]){""}}
///Calibration Temp (K)
#define SDFITS_COLUMN_TCAL (extfits_column){"TCAL", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "Calibration Temp", (char*[]){""}}
///Hot load temperature (K)
#define SDFITS_COLUMN_THOT (extfits_column){"THOT", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "Hot load temperature", (char*[]){""}}
///Cold load temperature (K)
#define SDFITS_COLUMN_TCOLD (extfits_column){"TCOLD", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "Cold load temperature", (char*[]){""}}
///Receiver Temp (K), Float
#define SDFITS_COLUMN_TRX (extfits_column){"TRX", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "Receiver Temp, Float", (char*[]){""}}
///Velocity definition & frame;
#define SDFITS_COLUMN_VELDEF (extfits_column){"VELDEF", "8A", "", "", "Velocity definition & frame", (char*[]){VELDEF_RADI, VELDEF_OPTI, VELDEF_RELA, VELDEF_LSR, VELDEF_HELO, VELDEF_EART, VELDEF_BARI, VELDEF_OBS, ""}}
///radial velocity correction; Vref - Vtel
#define SDFITS_COLUMN_VCORR (extfits_column){"VCORR", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "radial velocity correction; Vref - Vtel", (char*[]){""}}
///Observed Frequency (Hz)
#define SDFITS_COLUMN_OBSFREQ (extfits_column){"OBSFREQ", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_HZ, "", "Observed Frequency", (char*[]){""}}
///Image sideband freq (Hz)
#define SDFITS_COLUMN_IMAGFREQ (extfits_column){"IMAGFREQ", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_HZ, "", "Image sideband freq", (char*[]){""}}
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST (extfits_column){"LST", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_SECOND, "", "LST at start of scan", (char*[]){""}}
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST (extfits_column){"LST", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_SECOND, "", "LST at start of scan", (char*[]){""}}
///Commanded Azimuth (Degrees)
#define SDFITS_COLUMN_AZIMUTH (extfits_column){"AZIMUTH", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Commanded Azimuth", (char*[]){""}}
///Commanded Elevation (Degrees)
#define SDFITS_COLUMN_ELEVATIO (extfits_column){"ELEVATIO", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Commanded Elevation", (char*[]){""}}
///Opacity at signal freq
#define SDFITS_COLUMN_TAU (extfits_column){"TAU", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_PERCENT, "", "Opacity at signal freq", (char*[]){""}}
///Opacity at image freq
#define SDFITS_COLUMN_TAUIMAGE (extfits_column){"TAUIMAGE", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_PERCENT, "", "Opacity at image freq", (char*[]){""}}
///Opacity per unit air mass
#define SDFITS_COLUMN_TAUZENIT (extfits_column){"TAUZENIT", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_PERCENT, "", "Opacity per unit air mass", (char*[]){""}}
///Decimal fraction 0..1
#define SDFITS_COLUMN_HUMIDITY (extfits_column){"HUMIDITY", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Decimal fraction 0..1", (char*[]){""}}
///Ambient Temp (K)
#define SDFITS_COLUMN_TAMBIENT (extfits_column){"TAMBIENT", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "Ambient Temp (K)", (char*[]){""}}
///Barometer reading mm Hg
#define SDFITS_COLUMN_PRESSURE (extfits_column){"PRESSURE", EXTFITS_ELEMENT_DOUBLE.typestr, "mm Hg", "", "Barometer reading ", (char*[]){""}}
///Dew point (K)
#define SDFITS_COLUMN_DEWPOINT (extfits_column){"DEWPOINT", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_KELVIN, "", "Dew point", (char*[]){""}}
///Wind speed m/s
#define SDFITS_COLUMN_WINDSPEE (extfits_column){"WINDSPEE", EXTFITS_ELEMENT_DOUBLE.typestr, "m/s", "", "Wind speed", (char*[]){""}}
///Degrees West of North
#define SDFITS_COLUMN_WINDDIRE (extfits_column){"WINDDIRE", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Degrees West of North", (char*[]){""}}
///Main-beam efficiency
#define SDFITS_COLUMN_BEAMEFF (extfits_column){"BEAMEFF", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_PERCENT, "", "Main-beam efficiency", (char*[]){""}}
///Antenna Aperature Efficiency
#define SDFITS_COLUMN_APEREFF (extfits_column){"APEREFF", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_PERCENT, "", "Antenna Aperature Efficiency", (char*[]){""}}
///Rear spillover
#define SDFITS_COLUMN_ETAL (extfits_column){"ETAL", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Rear spillover", (char*[]){""}}
///Accounts for forward loss
#define SDFITS_COLUMN_ETAFSS (extfits_column){"ETAFSS", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Accounts for forward loss", (char*[]){""}}
///K per Jy
#define SDFITS_COLUMN_ANTGAIN (extfits_column){"ANTGAIN", EXTFITS_ELEMENT_DOUBLE.typestr, "K/Jy", "", "", (char*[]){""}}
///Large main-beam FWHM
#define SDFITS_COLUMN_BMAJ (extfits_column){"BMAJ", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Large main-beam FWHM", (char*[]){""}}
///Small main-beam FWHM
#define SDFITS_COLUMN_BMIN (extfits_column){"BMIN", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "Small main-beam FWHM", (char*[]){""}}
///Beam position angle, measured East of North
#define SDFITS_COLUMN_BPA (extfits_column){"BPA", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Beam position angle", (char*[]){""}}
///Site longitude (Degrees)
#define SDFITS_COLUMN_SITELONG (extfits_column){"SITELONG", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Site longitude", (char*[]){""}}
///Site latitude (Degrees)
#define SDFITS_COLUMN_SITELAT (extfits_column){"SITELAT", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_DEGREE, "", "Site latitude", (char*[]){""}}
///site elevation in meters
#define SDFITS_COLUMN_SITEELEV (extfits_column){"SITEELEV", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_METER, "", "site elevation", (char*[]){""}}
///Epoch of observation (year)
#define SDFITS_COLUMN_EPOCH (extfits_column){"EPOCH", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_YEAR, "", "Epoch of observation", (char*[]){""}}
///Equinox of coords (year)
#define SDFITS_COLUMN_EQUINOX (extfits_column){"EQUINOX", EXTFITS_ELEMENT_DOUBLE.typestr, EXTFITS_MEASURE_UNIT_YEAR, "", "Equinox of coords", (char*[]){""}}

#define SDFITS_TABLE_MAIN (extfits_column[]){\
SDFITS_COLUMN_OBJECT,\
SDFITS_COLUMN_TELESCOP,\
SDFITS_COLUMN_FREQRES,\
SDFITS_COLUMN_BANDWID,\
SDFITS_COLUMN_DATE_OBS,\
SDFITS_COLUMN_TIME,\
SDFITS_COLUMN_EXPOSURE,\
SDFITS_COLUMN_TSYS,\
SDFITS_COLUMN_DATA,\
SDFITS_COLUMN_OBSERVER, \
SDFITS_COLUMN_OBSID, \
SDFITS_COLUMN_PROJID, \
SDFITS_COLUMN_SCAN, \
SDFITS_COLUMN_OBSMODE, \
SDFITS_COLUMN_MOLECULE, \
SDFITS_COLUMN_TRANSITI, \
SDFITS_COLUMN_TEMPSCAL, \
SDFITS_COLUMN_FRONTEND, \
SDFITS_COLUMN_TCAL, \
SDFITS_COLUMN_THOT, \
SDFITS_COLUMN_TCOLD, \
SDFITS_COLUMN_TRX, \
SDFITS_COLUMN_VELDEF, \
SDFITS_COLUMN_VCORR, \
SDFITS_COLUMN_OBSFREQ, \
SDFITS_COLUMN_IMAGFREQ, \
SDFITS_COLUMN_LST, \
SDFITS_COLUMN_LST, \
SDFITS_COLUMN_AZIMUTH, \
SDFITS_COLUMN_ELEVATIO, \
SDFITS_COLUMN_TAU, \
SDFITS_COLUMN_TAUIMAGE, \
SDFITS_COLUMN_TAUZENIT, \
SDFITS_COLUMN_HUMIDITY, \
SDFITS_COLUMN_TAMBIENT, \
SDFITS_COLUMN_PRESSURE, \
SDFITS_COLUMN_DEWPOINT, \
SDFITS_COLUMN_WINDSPEE, \
SDFITS_COLUMN_WINDDIRE, \
SDFITS_COLUMN_BEAMEFF, \
SDFITS_COLUMN_APEREFF, \
SDFITS_COLUMN_ETAL, \
SDFITS_COLUMN_ETAFSS, \
SDFITS_COLUMN_ANTGAIN, \
SDFITS_COLUMN_BMAJ, \
SDFITS_COLUMN_BMIN, \
SDFITS_COLUMN_BPA, \
SDFITS_COLUMN_SITELONG, \
SDFITS_COLUMN_SITELAT, \
SDFITS_COLUMN_SITEELEV, \
SDFITS_COLUMN_EPOCH, \
SDFITS_COLUMN_EQUINOX, \
}

///Global keywords for the SDFITS SINGLE DISH table
///Designation of Telescope.
#define SDFITS_KEYWORD_TELESCOP (extfits_keyword){"TELESCOP", "8A", "", "", "", (char*[]){""}}
///Name of observer.
#define SDFITS_KEYWORD_OBSERVER (extfits_keyword){"OBSERVER", "8A", "", "", "", (char*[]){""}}
///UT date of observation (dd/mm/yy) .
#define SDFITS_KEYWORD_DATE_OBS (extfits_keyword){"DATE-OBS", "8A", "", "", "", (char*[]){""}}
///Max spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMAX (extfits_keyword){"DATAMAX", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "", (char*[]){""}}
///Min spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMIN (extfits_keyword){"DATAMIN", EXTFITS_ELEMENT_DOUBLE.typestr, "", "", "", (char*[]){""}}

#define FITS_KEYWORD_EXTEND (extfits_keyword){"EXTEND", "A", "", "T", "", (char*[]){""}}

void extfits_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status);
long extfits_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows);
int extfits_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, unsigned long num_rows);
int extfits_append_fits_col(fitsfile *fptr, char* name, char* format);
void extfits_delete_fits_col(fitsfile *fptr, char* name);
size_t extfits_get_element_size(int typecode);
int extfits_read_typecode(char* typestr, int *typecode, long *width, long *repeat);
int extfits_get_value(fitsfile *fptr, char* column, long rown, void **retval);
int extfits_check_column(fitsfile *fptr, char* column, char **expected, long rown);
fitsfile* extfits_create_fits_(size_t *size, void **buf);
int extfits_close_fits(fitsfile *fptr);
extfits_row *extfits_read_sdfits(char *filename, long *nstreams, long *maxes, long **maxis);

#ifdef __cplusplus
}
#endif

#endif //_FITS_EXTENSIONS_H
