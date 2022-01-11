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
    char *name;
    char *format;
    char *unit;
    char *value;
    char *comment;
    char **expected;
    int typecode;
    long repeat;
    long width;
} fitsext_column;

///Returns non-zero decimal conversion of integer into string with a prefix
#ifndef itostr
#define itostr(x) #x
#endif

///Antenna polarizations
///I
#define FITS_STOKE_I "1"
///Q
#define FITS_STOKE_Q "2"
///U
#define FITS_STOKE_U "3"
///V
#define FITS_STOKE_V "4"
///RR
#define FITS_STOKE_RR "-1"
///LL
#define FITS_STOKE_LL "-2"
///RL
#define FITS_STOKE_RL "-3"
///LR
#define FITS_STOKE_LR "-4"
///XX
#define FITS_STOKE_XX "-5"
///YY
#define FITS_STOKE_YY "-6"
///XY
#define FITS_STOKE_XY "-7"
///YX
#define FITS_STOKE_YX "-8"

///FITS element types
#define FITS_ELEMENT_CHARACTER_STRING "A"
#define FITS_ELEMENT_LOGICAL "L"
#define FITS_ELEMENT_BIT "X"
#define FITS_ELEMENT_UNSIGNED_BYTE "B"
#define FITS_ELEMENT_SIGNED_BYTE "S"
#define FITS_ELEMENT_SIGNED_INT_16_BIT "I"
#define FITS_ELEMENT_UNSIGNED_INT_16_BIT "U"
#define FITS_ELEMENT_SIGNED_INT_32_BIT "J"
#define FITS_ELEMENT_UNSIGNED_INT_32_BIT "V"
#define FITS_ELEMENT_SIGNED_INT_64_BIT "K"
#define FITS_ELEMENT_FLOATING_POINT_32_BIT "E"
#define FITS_ELEMENT_FLOATING_POINT_64_BIT "D"
#define FITS_ELEMENT_COMPLEX_PAIR_32_BIT "C"
#define FITS_ELEMENT_COMPLEX_PAIR_64_BIT "M"
#define FITS_ELEMENT_POINTER_128_BIT "P"

///FITS Measure units
#define FITS_MEASURE_UNIT_HZ "Hz"
#define FITS_MEASURE_UNIT_SECOND "sec"
#define FITS_MEASURE_UNIT_MINUTE "min"
#define FITS_MEASURE_UNIT_HOUR "hour"
#define FITS_MEASURE_UNIT_DAY "day"
#define FITS_MEASURE_UNIT_MONTH "month"
#define FITS_MEASURE_UNIT_YEAR "year"
#define FITS_MEASURE_UNIT_JANSKY "Jy"
#define FITS_MEASURE_UNIT_KELVIN "K"
#define FITS_MEASURE_UNIT_ANGSTROM "Angstrom"
#define FITS_MEASURE_UNIT_ARCSEC "arcsec"
#define FITS_MEASURE_UNIT_ARCMIN "arcmin"
#define FITS_MEASURE_UNIT_DEGREE "degree"
#define FITS_MEASURE_UNIT_PERCENT "percent"
#define FITS_MEASURE_UNIT_METER "meter"
#define FITS_MEASURE_UNIT_MILLIBAR "millibar"

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
#define FITSIDI_COLUMN_CORRELAT (fitsext_column){"CORRELAT", FITS_ELEMENT_CHARACTER_STRING, "", "", "Name/type of correlator", {""}, 0, 0, 0}
///Version number of the correlator software that produced the file
#define FITSIDI_COLUMN_FXCORVER (fitsext_column){"FXCORVER", FITS_ELEMENT_CHARACTER_STRING, "", "", "Version number of the correlator software that produced the file", {""}, 0, 0, 0}

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
#define FITSIDI_UV_DATA_AXIS_COMPLEX (fitsext_column){"COMPLEX", "", "", "", "Real, imaginary, weight", {""}, 0, 0, 0}
///Stokes parameter
#define FITSIDI_UV_DATA_AXIS_STOKES (fitsext_column){"STOKES", "", "", "", "Stokes parameter", {""}, 0, 0, 0}
///Frequency (spectral channel)
#define FITSIDI_UV_DATA_AXIS_FREQ (fitsext_column){"FREQ", "", "", "", "Frequency (spectral channel)", {""}, 0, 0, 0}
///Band number
#define FITSIDI_UV_DATA_AXIS_BAND (fitsext_column){"BAND", "", "", "", "Band number", {""}, 0, 0, 0}
///Right ascension of the phase center
#define FITSIDI_UV_DATA_AXIS_RA (fitsext_column){"RA", "", "", "", "Right ascension of the phase center", {""}, 0, 0, 0}
///Declination of the phase center
#define FITSIDI_UV_DATA_AXIS_DEC (fitsext_column){"DEC", "", "", "", "Declination of the phase center", {""}, 0, 0, 0}

///Random parameters for the FITS-IDI UV_DATA table
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU (fitsext_column){"UU", "1D", FITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-SIN system)", {""}, 0, 0, 0}
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV (fitsext_column){"VV", "1D", FITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-SIN system)", {""}, 0, 0, 0}
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW (fitsext_column){"WW", "1D", FITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-SIN system)", {""}, 0, 0, 0}
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU_SIN (fitsext_column){"UU---SIN", "1D", FITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-SIN system)", {""}, 0, 0, 0}
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV_SIN (fitsext_column){"VV---SIN", "1D", FITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-SIN system)", {""}, 0, 0, 0}
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW_SIN (fitsext_column){"WW---SIN", "1D", FITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-SIN system)", {""}, 0, 0, 0}
///seconds u baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_UU_NCP (fitsext_column){"UU---NCP", "1D", FITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-NCP system)", {""}, 0, 0, 0}
///seconds v baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_VV_NCP (fitsext_column){"VV---NCP", "1D", FITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-NCP system)", {""}, 0, 0, 0}
///seconds w baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_WW_NCP (fitsext_column){"WW---NCP", "1D", FITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-NCP system)", {""}, 0, 0, 0}
///days Julian date at 0 hours
#define FITSIDI_UV_DATA_COLUMN_DATE (fitsext_column){"DATE", "1D", FITS_MEASURE_UNIT_DAY, "", "Julian date at 0 hours", {""}, 0, 0, 0}
///days Time elapsed since 0 hours
#define FITSIDI_UV_DATA_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Time elapsed since 0 hours", {""}, 0, 0, 0}
///Baseline number
#define FITSIDI_UV_DATA_COLUMN_BASELINE (fitsext_column){"BASELINE", "1J", "", "", "Baseline number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_UV_DATA_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_UV_DATA_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Frequency setup ID number
#define FITSIDI_UV_DATA_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup ID number", {""}, 0, 0, 0}
///seconds Integration time
#define FITSIDI_UV_DATA_COLUMN_INTTIM (fitsext_column){"INTTIM", "1D", FITS_MEASURE_UNIT_SECOND, "", "Integration time", {""}, 0, 0, 0}
///Weights
#define FITSIDI_UV_DATA_COLUMN_WEIGHT(nstokes, nband) (fitsext_column){"WEIGHT", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nstokes) "," itostr(nband), "", "", "Weights", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI UV_DATA table
///2
#define FITSIDI_UV_DATA_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2", {""}, 0, 0, 0}

///NMATRIX shall be present with the value 1
#define FITSIDI_UV_DATA_KEYWORD_NMATRIX (fitsext_column){"NMATRIX", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "NMATRIX shall be present with the value 1", {""}, 0, 0, 0}
///M = number axes in regular matrix, Number pixels on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_MAXIS(m) (fitsext_column){"MAXIS" itostr(m), FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "M = number axes in regular matrix, Number pixels on axis m = 1 to M", {""}, 0, 0, 0}
///Set to 'T' — column n contains the visibility matrix
#define FITSIDI_UV_DATA_KEYWORD_TMATX(n) (fitsext_column){"TMATX" itostr(n), FITS_ELEMENT_CHARACTER_STRING, "", "", "Set to 'T'", {""}, 0, 0, 0}
///Set to 'FLUX'
#define FITSIDI_UV_DATA_KEYWORD_TTYPE(n) (fitsext_column){"TTYPE" itostr(n), "8A", "", "FLUX", "Set to 'FLUX'", {""}, 0, 0, 0}
///shall have the value 'JY' or 'UNCALIB'
#define FITSIDI_UV_DATA_KEYWORD_TUNIT(n) (fitsext_column){"TUNIT" itostr(n), "8A", "", "UNCALIB", "Shall have the value 'JY' or 'UNCALIB'", {""}, 0, 0, 0}
///Name of regular axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CTYPE(m) (fitsext_column){"CTYPE" itostr(m), FITS_ELEMENT_CHARACTER_STRING, "", "", "Name of regular axis m = 1 to M", {""}, 0, 0, 0}
///Coordinate increment on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CDELT(m) (fitsext_column){"CDELT" itostr(m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate increment on axis m = 1 to M", {""}, 0, 0, 0}
///Reference pixel on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CRPIX(m) (fitsext_column){"CRPIX" itostr(m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Reference pixel on axis m = 1 to M", {""}, 0, 0, 0}
///Coordinate value at reference pixel on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CRVAL(m) (fitsext_column){"CRVAL" itostr(m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate value at reference pixel on axis m = 1 to M", {""}, 0, 0, 0}
///The number of Stokes parameters
#define FITSIDI_UV_DATA_KEYWORD_NO_STKD (fitsext_column){"NO_STKD", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The number of Stokes parameters", {""}, 0, 0, 0}
///The first Stokes parameter coordinate value
#define FITSIDI_UV_DATA_KEYWORD_STK_1 (fitsext_column){"STK_1", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The first Stokes parameter coordinate value", {""}, 0, 0, 0}
///The number of bands
#define FITSIDI_UV_DATA_KEYWORD_NO_BAND (fitsext_column){"NO_BAND", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The number of bands", {""}, 0, 0, 0}
///The number of spectral channels per band
#define FITSIDI_UV_DATA_KEYWORD_NO_CHAN (fitsext_column){"NO_CHAN", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The number of spectral channels per band", {""}, 0, 0, 0}
///The file reference frequency in Hz
#define FITSIDI_UV_DATA_KEYWORD_REF_FREQ (fitsext_column){"REF_FREQ", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "The file reference frequency in Hz", {""}, 0, 0, 0}
///The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1
#define FITSIDI_UV_DATA_KEYWORD_CHAN_BW (fitsext_column){"CHAN_BW", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1", {""}, 0, 0, 0}
///The reference pixel for the frequency axis
#define FITSIDI_UV_DATA_KEYWORD_REF_PIXL (fitsext_column){"REF_PIXL", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "The reference pixel for the frequency axis", {""}, 0, 0, 0}
///Mean equinox
#define FITSIDI_UV_DATA_KEYWORD_EQUINOX (fitsext_column){"EQUINOX", "8A", "", "", "Mean equinox", {""}, 0, 0, 0}
///Type of data weights
#define FITSIDI_UV_DATA_KEYWORD_WEIGHTYP (fitsext_column){"WEIGHTYP", "8A", "", "", "Type of data weights", {""}, 0, 0, 0}

///Columns for the FITS-IDI ARRAY_GEOMETRY table
///Antenna name
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ANNAME (fitsext_column){"ANNAME", "8A", "", "", "Antenna name", {""}, 0, 0, 0}
///meters Antenna station coordinates (x, y, z)
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STABXYZ (fitsext_column){"STABXYZ", "3D", FITS_MEASURE_UNIT_METER, "", "Antenna station coordinates (x, y, z)", {""}, 0, 0, 0}
///meters/s First-order derivatives of the station coordinates with respect to time
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DERXYZ (fitsext_column){"DERXYZ", "3E", "meters/s", "", "First-order derivatives of the station coordinates with respect to time", {""}, 0, 0, 0}
///Orbital parameters
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ORBPARM(norb) (fitsext_column){"ORBPARM", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(norb), "", "", "Orbital parameters", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_NOSTA (fitsext_column){"NOSTA", "1I", "", "", "Antenna number", {""}, 0, 0, 0}
///Mount type
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_MNTSTA (fitsext_column){"MNTSTA", "1J", "", "", "Mount type", {""}, 0, 0, 0}
///meters Axis offset
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STAXOF (fitsext_column){"STAXOF", "3E", FITS_MEASURE_UNIT_METER, "", "Axis offset", {""}, 0, 0, 0}
///meters Antenna diameter
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DIAMETER (fitsext_column){"DIAMETER", "1E", FITS_MEASURE_UNIT_METER, "", "Antenna diameter", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI ARRAY_GEOMETRY table
///1
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Array number
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_EXTVER (fitsext_column){"EXTVER", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Array number", {""}, 0, 0, 0}
///Array name
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRNAM (fitsext_column){"ARRNAM", FITS_ELEMENT_CHARACTER_STRING, "", "", "Array name", {""}, 0, 0, 0}
///Coordinate frame
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FRAME (fitsext_column){"FRAME", FITS_ELEMENT_CHARACTER_STRING, "", "", "Coordinate frame", {""}, 0, 0, 0}
///x coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYX (fitsext_column){"ARRAYX", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "x coordinate of array center (m)", {""}, 0, 0, 0}
///y coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYY (fitsext_column){"ARRAYY", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "y coordinate of array center (m)", {""}, 0, 0, 0}
///z coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYZ (fitsext_column){"ARRAYZ", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "z coordinate of array center (m)", {""}, 0, 0, 0}
///norb= number orbital parameters in table
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_NUMORB (fitsext_column){"NUMORB", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "norb= number orbital parameters in table", {""}, 0, 0, 0}
///Reference frequency (Hz)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FREQ (fitsext_column){"FREQ", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Reference frequency (Hz)", {""}, 0, 0, 0}
///Time system
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TIMESYS (fitsext_column){"TIMESYS", FITS_ELEMENT_CHARACTER_STRING, "", "", "Time system", {""}, 0, 0, 0}
///Reference date
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_RDATE (fitsext_column){"RDATE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Reference date", {""}, 0, 0, 0}
///GST at 0h on reference date (degrees)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_GSTIA0 (fitsext_column){"GSTIA0", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "GST at 0h on reference date (degrees)", {""}, 0, 0, 0}
///Earth's rotation rate (degrees/day)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_DEGPDY (fitsext_column){"DEGPDY", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Earth's rotation rate (degrees/day)", {""}, 0, 0, 0}
///UT1 - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_UT1UTC (fitsext_column){"UT1UTC", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "UT1 - UTC (sec)", {""}, 0, 0, 0}
///IAT - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_IATUTC (fitsext_column){"IATUTC", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "IAT - UTC (sec)", {""}, 0, 0, 0}
///x coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARX (fitsext_column){"POLARX", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "x coordinate of North Pole (arc seconds)", {""}, 0, 0, 0}
///y coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARY (fitsext_column){"POLARY", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "y coordinate of North Pole (arc seconds)", {""}, 0, 0, 0}

///Columns for the FITS-IDI ANTENNA table
///days Central time of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of period covered by record", {""}, 0, 0, 0}
///days Duration of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of period covered by record", {""}, 0, 0, 0}
///Antenna name
#define FITSIDI_ANTENNA_COLUMN_ANNAME (fitsext_column){"ANNAME", "8A", "", "", "Antenna name", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_ANTENNA_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_ANTENNA_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_ANTENNA_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Number of digitizer levels
#define FITSIDI_ANTENNA_COLUMN_NO_LEVELS (fitsext_column){"NO_LEVELS", "1J", "", "", "Number of digitizer levels", {""}, 0, 0, 0}
///Feed A polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYA (fitsext_column){"POLTYA", "1A", "Feed A polarization label", {""}, 0, 0, 0}
///degrees Feed A orientation
#define FITSIDI_ANTENNA_COLUMN_POLAA(nband) (fitsext_column){"POLAA", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_DEGREE, "", "Feed A orientation", {""}, 0, 0, 0}
///Feed A polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALA(npcal, nband) (fitsext_column){"POLCALA", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(npcal) "," itostr(nband), "", "", "Feed A polarization parameters", {""}, 0, 0, 0}
///Feed B polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYB (fitsext_column){"POLTYB", "1A", "Feed B polarization label", {""}, 0, 0, 0}
///degrees Feed B orientation
#define FITSIDI_ANTENNA_COLUMN_POLAB(nband) (fitsext_column){"POLAB", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_DEGREE, "", "Feed B orientation", {""}, 0, 0, 0}
///Feed B polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALB(npcal, nband) (fitsext_column){"POLCALB", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(npcal) "," itostr(nband), "", "", "Feed B polarization parameters", {""}, 0, 0, 0}
///degrees / m Antenna beam fwhm
#define FITSIDI_ANTENNA_COLUMN_BEAMFWHM(nband) (fitsext_column){"BEAMFWHM", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_DEGREE, "", "/ m Antenna beam fwhm", {""}, 0, 0, 0}

///Polarization parameters
///Linear approximation for circular feeds
#define FITSIDI_ANTENNA_POLPARM_APPROX "APPROX"
///Linear approximation for linear feeds
#define FITSIDI_ANTENNA_POLPARM_LIN "X-Y LIN"
///Orientation and ellipticity
#define FITSIDI_ANTENNA_POLPARM_ORI_ELP "ORI-ELP"

///Mandatory keywords for the FITS-IDI ANTENNA table
///1
#define FITSIDI_ANTENNA_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///npcal = 0 or 2, number of polarization calibration constants
#define FITSIDI_ANTENNA_KEYWORD_NOPCAL (fitsext_column){"NOPCAL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "npcal = 0 or 2, number of polarization calibration constants", {""}, 0, 0, 0}
///The feed polarization parameterization
#define FITSIDI_ANTENNA_KEYWORD_POLTYPE (fitsext_column){"POLTYPE", FITS_ELEMENT_CHARACTER_STRING, "", "", "The feed polarization parameterization", {""}, 0, 0, 0}

///Columns for the FITS-IDI FREQUENCY table
///Frequency setup number
#define FITSIDI_FREQUENCY_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Hz Frequency offsets
#define FITSIDI_FREQUENCY_COLUMN_BANDFREQ(nband) (fitsext_column){"BANDFREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Frequency offsets", {""}, 0, 0, 0}
///Hz Individual channel widths
#define FITSIDI_FREQUENCY_COLUMN_CH_WIDTH(nband) (fitsext_column){"CH_WIDTH", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Individual channel widths", {""}, 0, 0, 0}
///Hz Total bandwidths of bands
#define FITSIDI_FREQUENCY_COLUMN_TOTAL_BANDWIDTH(nband) (fitsext_column){"TOTAL_BANDWIDTH", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Total bandwidths of bands", {""}, 0, 0, 0}
///Sideband flag
#define FITSIDI_FREQUENCY_COLUMN_SIDEBAND(nband) (fitsext_column){"SIDEBAND", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Sideband flag", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI FREQUENCY table
///1
#define FITSIDI_FREQUENCY_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}

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
#define FITSIDI_SOURCE_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Source name
#define FITSIDI_SOURCE_COLUMN_SOURCE (fitsext_column){"SOURCE", "16A", "", "", "Source name", {""}, 0, 0, 0}
///Source name numeric qualifier
#define FITSIDI_SOURCE_COLUMN_QUAL (fitsext_column){"QUAL", "1J", "", "", "Source name numeric qualifier", {""}, 0, 0, 0}
///Calibrator code
#define FITSIDI_SOURCE_COLUMN_CALCODE (fitsext_column){"CALCODE", "4A", "", "", "Calibrator code", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_SOURCE_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Jy Stokes I flux density
#define FITSIDI_SOURCE_COLUMN_IFLUX(nband) (fitsext_column){"IFLUX", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Jy", "", "Stokes I flux density", {""}, 0, 0, 0}
///Jy Stokes Q flux density
#define FITSIDI_SOURCE_COLUMN_QFLUX(nband) (fitsext_column){"QFLUX", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Jy", "", "Stokes Q flux density", {""}, 0, 0, 0}
///Jy Stokes U flux density
#define FITSIDI_SOURCE_COLUMN_UFLUX(nband) (fitsext_column){"UFLUX", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Jy", "", "Stokes U flux density", {""}, 0, 0, 0}
///Jy Stokes V flux density
#define FITSIDI_SOURCE_COLUMN_VFLUX(nband) (fitsext_column){"VFLUX", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Jy", "", "Stokes V flux density", {""}, 0, 0, 0}
///Jy Spectral index for each band
#define FITSIDI_SOURCE_COLUMN_ALPHA(nband) (fitsext_column){"ALPHA", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Jy", "", "Spectral index for each band", {""}, 0, 0, 0}
///Hz Frequency offset for each band
#define FITSIDI_SOURCE_COLUMN_FREQOFF(nband) (fitsext_column){"FREQOFF", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Frequency offset for each band", {""}, 0, 0, 0}
///degrees Right ascension at mean equinox
#define FITSIDI_SOURCE_COLUMN_RAEPO (fitsext_column){"RAEPO", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Right ascension at mean equinox", {""}, 0, 0, 0}
///degrees Declination at mean equinox
#define FITSIDI_SOURCE_COLUMN_DECEPO (fitsext_column){"DECEPO", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Declination at mean equinox", {""}, 0, 0, 0}
///Mean equinox
#define FITSIDI_SOURCE_COLUMN_EQUINOX (fitsext_column){"EQUINOX", "8A", "", "", "Mean equinox", {""}, 0, 0, 0}
///degrees Apparent right ascension
#define FITSIDI_SOURCE_COLUMN_RAAPP (fitsext_column){"RAAPP", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Apparent right ascension", {""}, 0, 0, 0}
///degrees Apparent declination
#define FITSIDI_SOURCE_COLUMN_DECAPP (fitsext_column){"DECAPP", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Apparent declination", {""}, 0, 0, 0}
///meters/sec Systemic velocity for each band
#define FITSIDI_SOURCE_COLUMN_SYSVEL(nband) (fitsext_column){"SYSVEL", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(nband), "meters/sec", "", "Systemic velocity for each band", {""}, 0, 0, 0}
///Velocity type
#define FITSIDI_SOURCE_COLUMN_VELTYP (fitsext_column){"VELTYP", "8A", "", "", "Velocity type", {""}, 0, 0, 0}
///Velocity definition
#define FITSIDI_SOURCE_COLUMN_VELDEF (fitsext_column){"VELDEF", "8A", "", "", "Velocity definition", {""}, 0, 0, 0}
///Hz Line rest frequency for each band
#define FITSIDI_SOURCE_COLUMN_RESTFREQ(nband) (fitsext_column){"RESTFREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Line rest frequency for each band", {""}, 0, 0, 0}
///degrees/day Proper motion in right ascension
#define FITSIDI_SOURCE_COLUMN_PMRA (fitsext_column){"PMRA", "1D", "degrees/day", "", "Proper motion in right ascension", {""}, 0, 0, 0}
///degrees/day Proper motion in declination
#define FITSIDI_SOURCE_COLUMN_PMDEC (fitsext_column){"PMDEC", "1D", "degrees/day", "", "Proper motion in declination", {""}, 0, 0, 0}
///arcseconds Parallax of source
#define FITSIDI_SOURCE_COLUMN_PARALLAX (fitsext_column){"PARALLAX", "1E", FITS_MEASURE_UNIT_ARCSEC, "", "Parallax of source", {""}, 0, 0, 0}
///years Epoch of observation
#define FITSIDI_SOURCE_COLUMN_EPOCH (fitsext_column){"EPOCH", "1D", FITS_MEASURE_UNIT_YEAR, "", "Epoch of observation", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI SOURCE table
///1
#define FITSIDI_SOURCE_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}

///Columns for the FITS-IDI INTERFEROMETER_MODEL table
///days Starting time of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Starting time of interval", {""}, 0, 0, 0}
///days Duration of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///rad m−2 Ionospheric Faraday rotation
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_I_FAR_ROT (fitsext_column){"I.FAR.ROT", "1E", "rad m−2", "", "Ionospheric Faraday rotation", {""}, 0, 0, 0}
///Hz Time variable frequency offsets
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQ_VAR(nband) (fitsext_column){"FREQ.VAR", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Time variable frequency offsets", {""}, 0, 0, 0}
///turns Phase delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_1(npoly, nband) (fitsext_column){"PDELAY_1", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), "turns", "", "Phase delay polynomials for polarization 1", {""}, 0, 0, 0}
///seconds Group delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_1(npoly, nband) (fitsext_column){"GDELAY_1", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), FITS_MEASURE_UNIT_SECOND, "", "Group delay polynomials for polarization 1", {""}, 0, 0, 0}
///Hz Phase delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_1(npoly, nband) (fitsext_column){"PRATE_1", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Phase delay rate polynomials for polarization 1", {""}, 0, 0, 0}
///sec/sec Group delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_1(npoly, nband) (fitsext_column){"GRATE_1", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), "sec/sec", "", "Group delay rate polynomials for polarization 1", {""}, 0, 0, 0}
///sec m−2 Dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_1 (fitsext_column){"DISP_1", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay for polarization 1", {""}, 0, 0, 0}
///sec m−2/sec Rate of change of dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_1 (fitsext_column){"DDISP_1", "1E", "sec m−2/sec", "", " Rate of change of dispersive delay for polarization 1", {""}, 0, 0, 0}
///turns Phase delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_2(npoly, nband) (fitsext_column){"PDELAY_2", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), "turns", "", "Phase delay polynomials for polarization 2", {""}, 0, 0, 0}
///seconds Group delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_2(npoly, nband) (fitsext_column){"GDELAY_2", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), FITS_MEASURE_UNIT_SECOND, "", "Group delay polynomials for polarization 2", {""}, 0, 0, 0}
///Hz Phase delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_2(npoly, nband) (fitsext_column){"PRATE_2", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Phase delay rate polynomials for polarization 2", {""}, 0, 0, 0}
///sec/sec Group delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_2(npoly, nband) (fitsext_column){"GRATE_2", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(npoly) "," itostr(nband), "sec/sec", "", "Group delay rate polynomials for polarization 2", {""}, 0, 0, 0}
///sec m−2 Dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_2 (fitsext_column){"DISP_2", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay for polarization 2", {""}, 0, 0, 0}
///sec m−2/sec Rate of change of dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_2 (fitsext_column){"DDISP_2", "1E", "sec m−2/sec", "", " Rate of change of dispersive delay for polarization 2", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI INTERFEROMETER_MODEL table
///2
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2", {""}, 0, 0, 0}
///Number of polynomial terms npoly
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NPOLY (fitsext_column){"NPOLY", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polynomial terms npoly", {""}, 0, 0, 0}
///Number of polarizations
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations", {""}, 0, 0, 0}

///Columns for the FITS-IDI SYSTEM_TEMPERATURE table
///days Central time of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///days Duration of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Kelvin System temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_1(nband) (fitsext_column){"TSYS_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperatures for polarization 1", {""}, 0, 0, 0}
///Kelvin Antenna temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_1(nband) (fitsext_column){"TANT_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperatures for polarization 1", {""}, 0, 0, 0}
///Kelvin System temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_2(nband) (fitsext_column){"TSYS_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperatures for polarization 2", {""}, 0, 0, 0}
///Kelvin Antenna temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_2(nband) (fitsext_column){"TANT_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperatures for polarization 2", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI SYSTEM_TEMPERATURE table
///1
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Number of polarizations in the table
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table", {""}, 0, 0, 0}

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
#define FITSIDI_GAIN_CURVE_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_GAIN_CURVE_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_GAIN_CURVE_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Gain curve types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_1(nband) (fitsext_column){"TYPE_1", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Gain curve types for polarization 1", {""}, 0, 0, 0}
///Number of terms or entries for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_1(nband) (fitsext_column){"NTERM_1", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Number of terms or entries for polarization 1", {""}, 0, 0, 0}
///x value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_1(nband) (fitsext_column){"X_TYP_1", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "x value types for polarization 1", {""}, 0, 0, 0}
///y value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_1(nband) (fitsext_column){"Y_TYP_1", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "y value types for polarization 1", {""}, 0, 0, 0}
///x values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_1(nband) (fitsext_column){"X_VAL_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "x values for polarization 1", {""}, 0, 0, 0}
///y values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_1(ntab, nband) (fitsext_column){"Y_VAL_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntab) "," itostr(nband), "", "", "y values for polarization 1", {""}, 0, 0, 0}
///Relative gain values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_1(ntab, nband) (fitsext_column){"GAIN_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntab) "," itostr(nband), "", "", "Relative gain values for polarization 1", {""}, 0, 0, 0}
///K/Jy Sensitivities for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_1(nband) (fitsext_column){"SENS_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "K/Jy", "", " Sensitivities for polarization 1", {""}, 0, 0, 0}
///Gain curve types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_2(nband) (fitsext_column){"TYPE_2", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Gain curve types for polarization 2", {""}, 0, 0, 0}
///Number of terms or entries for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_2(nband) (fitsext_column){"NTERM_2", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Number of terms or entries for polarization 2", {""}, 0, 0, 0}
///x value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_2(nband) (fitsext_column){"X_TYP_2", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "x value types for polarization 2", {""}, 0, 0, 0}
///y value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_2(nband) (fitsext_column){"Y_TYP_2", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "y value types for polarization 2", {""}, 0, 0, 0}
///x values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_2(nband) (fitsext_column){"X_VAL_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "x values for polarization 2", {""}, 0, 0, 0}
///y values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_2(ntab, nband) (fitsext_column){"Y_VAL_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntab) "," itostr(nband), "", "", "y values for polarization 2", {""}, 0, 0, 0}
///Relative gain values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_2(ntab, nband) (fitsext_column){"GAIN_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntab) "," itostr(nband), "", "", "Relative gain values for polarization 2", {""}, 0, 0, 0}
///K/Jy Sensitivities for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_2(nband) (fitsext_column){"SENS_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "K/Jy", "", " Sensitivities for polarization 2", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI GAIN_CURVE table
///1
#define FITSIDI_GAIN_CURVE_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Number of polarizations in the table
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table", {""}, 0, 0, 0}
///Number of tabulated values ntab
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_TABS (fitsext_column){"NO_TABS", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of tabulated values ntab", {""}, 0, 0, 0}

///Columns for the FITS-IDI PHASE-CAL table
///days Central time of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///days Duration of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_PHASE_CAL_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_PHASE_CAL_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_PHASE_CAL_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_PHASE_CAL_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///seconds Cable calibration measurement
#define FITSIDI_PHASE_CAL_COLUMN_CABLE_CAL (fitsext_column){"CABLE_CAL", "1D", FITS_MEASURE_UNIT_SECOND, "", "Cable calibration measurement", {""}, 0, 0, 0}
///percent State counts for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_STATE_1(nband) (fitsext_column){"STATE_1", "E4,", nband), FITS_MEASURE_UNIT_PERCENT, "", "State counts for polarization 1", {""}, 0, 0, 0}
///Hz Phase-cal tone frequencies for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_1(ntone, nband) (fitsext_column){"PC_FREQ_1", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(ntone) "," itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Phase-cal tone frequencies for polarization 1", {""}, 0, 0, 0}
///Real parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_1(ntone, nband) (fitsext_column){"PC_REAL_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntone) "," itostr(nband), "", "", "Real parts of phase-cal measurements for polarization 1", {""}, 0, 0, 0}
///Imaginary parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_1(ntone, nband) (fitsext_column){"PC_IMAG_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntone) "," itostr(nband), "", "", "Imaginary parts of phase-cal measurements for polarization 1", {""}, 0, 0, 0}
///sec/sec Phase-cal rates for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_1(ntone, nband) (fitsext_column){"PC_RATE_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntone) "," itostr(nband), "sec/sec", "", "Phase-cal rates for polarization 1", {""}, 0, 0, 0}
///percent State counts for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_STATE_2(nband) (fitsext_column){"STATE_2", "E4,", nband), FITS_MEASURE_UNIT_PERCENT, "", "State counts for polarization 2", {""}, 0, 0, 0}
///Hz Phase-cal tone frequencies for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_2(ntone, nband) (fitsext_column){"PC_FREQ_2", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(ntone) "," itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Phase-cal tone frequencies for polarization 2", {""}, 0, 0, 0}
///Real parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_2(ntone, nband) (fitsext_column){"PC_REAL_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntone) "," itostr(nband), "", "", "Real parts of phase-cal measurements for polarization 2", {""}, 0, 0, 0}
///Imaginary parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_2(ntone, nband) (fitsext_column){"PC_IMAG_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntone) "," itostr(nband), "", "", "Imaginary parts of phase-cal measurements for polarization 2", {""}, 0, 0, 0}
///sec/sec Phase-cal rates for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_2(ntone, nband) (fitsext_column){"PC_RATE_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(ntone) "," itostr(nband), "sec/sec", "", "Phase-cal rates for polarization 2", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI PHASE-CAL table
///2
#define FITSIDI_PHASE_CAL_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2", {""}, 0, 0, 0}
///Number of polarizations in the table
#define FITSIDI_PHASE_CAL_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table", {""}, 0, 0, 0}
///Number of tones ntone
#define FITSIDI_PHASE_CAL_KEYWORD_NO_TABS (fitsext_column){"NO_TABS", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of tones ntone", {""}, 0, 0, 0}

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
#define FITSIDI_FLAG_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_FLAG_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Antenna numbers
#define FITSIDI_FLAG_COLUMN_ANTS (fitsext_column){"ANTS", "2J", "", "", "Antenna numbers", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_FLAG_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///days Time range
#define FITSIDI_FLAG_COLUMN_TIMERANG (fitsext_column){"TIMERANG", "2E", FITS_MEASURE_UNIT_DAY, "", "Time range", {""}, 0, 0, 0}
///Band flags
#define FITSIDI_FLAG_COLUMN_BANDS(nband) (fitsext_column){"BANDS", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Band flags", {""}, 0, 0, 0}
///Channel range
#define FITSIDI_FLAG_COLUMN_CHANS (fitsext_column){"CHANS", "2J", "", "", "Channel range", {""}, 0, 0, 0}
///Polarization flags
#define FITSIDI_FLAG_COLUMN_PFLAGS (fitsext_column){"PFLAGS", "4J", "", "", "Polarization flags", {""}, 0, 0, 0}
///Reason for flag
#define FITSIDI_FLAG_COLUMN_REASON(n) (fitsext_column){"REASON" itostr(n), FITS_ELEMENT_CHARACTER_STRING, "", "", "Reason for flag", {""}, 0, 0, 0}
///Severity code
#define FITSIDI_FLAG_COLUMN_SEVERITY (fitsext_column){"SEVERITY", "1J", "", "", "Severity code", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI FLAG table
///2
#define FITSIDI_FLAG_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2", {""}, 0, 0, 0}

///Columns for the FITS-IDI WEATHER table
///days Central time of interval
#define FITSIDI_WEATHER_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///days Duration of interval
#define FITSIDI_WEATHER_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_WEATHER_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Centigrade Surface air temperature
#define FITSIDI_WEATHER_COLUMN_TEMPERATURE (fitsext_column){"TEMPERATURE", "1E", "", "", "Centigrade Surface air temperature", {""}, 0, 0, 0}
///millibar Surface air pressure
#define FITSIDI_WEATHER_COLUMN_PRESSURE (fitsext_column){"PRESSURE", "1E", "millibar", "", "Surface air pressure", {""}, 0, 0, 0}
///Centigrade Dewpoint temperature
#define FITSIDI_WEATHER_COLUMN_DEWPOINT (fitsext_column){"DEWPOINT", "1E", "", "", "Centigrade Dewpoint temperature", {""}, 0, 0, 0}
///m s−1 Wind velocity
#define FITSIDI_WEATHER_COLUMN_WIND_VELOCITY (fitsext_column){"WIND_VELOCITY", "1E", "m s−1", "", " Wind velocity", {""}, 0, 0, 0}
///degrees Wind direction East from North
#define FITSIDI_WEATHER_COLUMN_WIND_DIRECTION (fitsext_column){"WIND_DIRECTION", "1E", FITS_MEASURE_UNIT_DEGREE, "", "Wind direction East from North", {""}, 0, 0, 0}
///m−2 Water column
#define FITSIDI_WEATHER_COLUMN_WVR_H2O (fitsext_column){"WVR_H2O", "1E", "m−2", "", "Water column", {""}, 0, 0, 0}
///m−2 Electron column
#define FITSIDI_WEATHER_COLUMN_IONOS_ELECTRON (fitsext_column){"IONOS_ELECTRON", "1E", "m−2", "", "Electron column", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI WEATHER table
///2
#define FITSIDI_WEATHER_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2", {""}, 0, 0, 0}
///Reference date
#define FITSIDI_WEATHER_KEYWORD_RDATE (fitsext_column){"RDATE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Reference date", {""}, 0, 0, 0}

///Columns for the FITS-IDI BASELINE table
///days Central time of interval
#define FITSIDI_BASELINE_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_BASELINE_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_BASELINE_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Antenna numbers forming baseline
#define FITSIDI_BASELINE_COLUMN_ANTENNA_NOS (fitsext_column){"ANTENNA_NOS.", "2J", "", "", "Antenna numbers forming baseline", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_BASELINE_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Real part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_REAL_M(nstokes, nband) (fitsext_column){"REAL_M", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nstokes) "," itostr(nband), "", "", "Real part of multiplicative correction", {""}, 0, 0, 0}
///Imaginary part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_IMAG_M(nstokes, nband) (fitsext_column){"IMAG_M", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nstokes) "," itostr(nband), "", "", "Imaginary part of multiplicative correction", {""}, 0, 0, 0}
///Real part of additive correction
#define FITSIDI_BASELINE_COLUMN_REAL_A(nstokes, nband) (fitsext_column){"REAL_A", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nstokes) "," itostr(nband), "", "", "Real part of additive correction", {""}, 0, 0, 0}
///Imaginary part of additive correction
#define FITSIDI_BASELINE_COLUMN_IMAG_A(nstokes, nband) (fitsext_column){"IMAG_A", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nstokes) "," itostr(nband), "", "", "Imaginary part of additive correction", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI BASELINE table
///1
#define FITSIDI_BASELINE_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Maximum antenna number in the table
#define FITSIDI_BASELINE_KEYWORD_NO_ANT (fitsext_column){"NO_ANT", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Maximum antenna number in the table", {""}, 0, 0, 0}

///Columns for the FITS-IDI BANDPASS table
///days Central time of interval
#define FITSIDI_BANDPASS_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///days Duration of interval
#define FITSIDI_BANDPASS_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_BANDPASS_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_BANDPASS_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_BANDPASS_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_BANDPASS_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Hz Channel bandwidth
#define FITSIDI_BANDPASS_COLUMN_BANDWIDTH (fitsext_column){"BANDWIDTH", "1E", FITS_MEASURE_UNIT_HZ, "", "Channel bandwidth", {""}, 0, 0, 0}
///Hz Frequency of each band
#define FITSIDI_BANDPASS_COLUMN_BAND_FREQ(nband) (fitsext_column){"BAND_FREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "Frequency of each band", {""}, 0, 0, 0}
///Reference antenna for polarization 1
#define FITSIDI_BANDPASS_COLUMN_REFANT_1 (fitsext_column){"REFANT_1", "1J", "", "", "Reference antenna for polarization 1", {""}, 0, 0, 0}
///Real part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BREAL_1(nbach, nband) (fitsext_column){"BREAL_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nbach) "," itostr(nband), "", "", "Real part of bandpass correction for polarization 1", {""}, 0, 0, 0}
///Imaginary part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BIMAG_1(nbach, nband) (fitsext_column){"BIMAG_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nbach) "," itostr(nband), "", "", "Imaginary part of bandpass correction for polarization 1", {""}, 0, 0, 0}
///Reference antenna for polarization 2
#define FITSIDI_BANDPASS_COLUMN_REFANT_2 (fitsext_column){"REFANT_2", "1J", "", "", "Reference antenna for polarization 2", {""}, 0, 0, 0}
///Real part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BREAL_2(nbach, nband) (fitsext_column){"BREAL_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nbach) "," itostr(nband), "", "", "Real part of bandpass correction for polarization 2", {""}, 0, 0, 0}
///Imaginary part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BIMAG_2(nbach, nband) (fitsext_column){"BIMAG_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nbach) "," itostr(nband), "", "", "Imaginary part of bandpass correction for polarization 2", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI BANDPASS table
///1
#define FITSIDI_BANDPASS_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Maximum antenna number in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_ANT (fitsext_column){"NO_ANT", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Maximum antenna number in the table", {""}, 0, 0, 0}
///Number of polarizations in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table", {""}, 0, 0, 0}
///Number of spectral channels in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_BACH (fitsext_column){"NO_BACH", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of spectral channels in the table", {""}, 0, 0, 0}
///Data channel number for first channel in the table
#define FITSIDI_BANDPASS_KEYWORD_STRT_CHN (fitsext_column){"STRT_CHN", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Data channel number for first channel in the table", {""}, 0, 0, 0}

///Columns for the FITS-IDI CALIBRATION table
///days Central time of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///days Duration of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME_INTERVAL (fitsext_column){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_CALIBRATION_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_CALIBRATION_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_CALIBRATION_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_CALIBRATION_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///Kelvin System temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TSYS_1(nband) (fitsext_column){"TSYS_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperature for polarization 1", {""}, 0, 0, 0}
///Kelvin Antenna temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TANT_1(nband) (fitsext_column){"TANT_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperature for polarization 1", {""}, 0, 0, 0}
///Kelvin/Jy Sensitivity at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_1(nband) (fitsext_column){"SENSITIVITY_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Kelvin/Jy", "", "Sensitivity at polarization 1", {""}, 0, 0, 0}
///radians Phase at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_PHASE_1(nband) (fitsext_column){"PHASE_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_RAD, "", "Phase at polarization 1", {""}, 0, 0, 0}
///sec/sec Rate of change of delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_RATE_1(nband) (fitsext_column){"RATE_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "sec/sec", "", "Rate of change of delay of polarization 1", {""}, 0, 0, 0}
///seconds Delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_DELAY_1(nband) (fitsext_column){"DELAY_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_SECOND, "", "Delay of polarization 1", {""}, 0, 0, 0}
///Complex gain real part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REAL_1(nband) (fitsext_column){"REAL_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "Complex gain real part for polarization 1", {""}, 0, 0, 0}
///Complex gain imaginary part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_IMAG_1(nband) (fitsext_column){"IMAG_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "Complex gain imaginary part for polarization 1", {""}, 0, 0, 0}
///Reliability weight of complex gain for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_1(nband) (fitsext_column){"WEIGHT_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "Reliability weight of complex gain for polarization 1", {""}, 0, 0, 0}
///Reference antenna for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REFANT_1(nband) (fitsext_column){"REFANT_1", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Reference antenna for polarization 1", {""}, 0, 0, 0}
///Kelvin System temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TSYS_2(nband) (fitsext_column){"TSYS_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperature for polarization 2", {""}, 0, 0, 0}
///Kelvin Antenna temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TANT_2(nband) (fitsext_column){"TANT_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperature for polarization 2", {""}, 0, 0, 0}
///Kelvin/Jy Sensitivity at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_2(nband) (fitsext_column){"SENSITIVITY_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Kelvin/Jy", "", "Sensitivity at polarization 2", {""}, 0, 0, 0}
///radians Phase at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_PHASE_2(nband) (fitsext_column){"PHASE_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_RAD, "", "Phase at polarization 2", {""}, 0, 0, 0}
///sec/sec Rate of change of delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_RATE_2(nband) (fitsext_column){"RATE_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "sec/sec", "", "Rate of change of delay of polarization 2", {""}, 0, 0, 0}
///seconds Delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_DELAY_2(nband) (fitsext_column){"DELAY_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_SECOND, "", "Delay of polarization 2", {""}, 0, 0, 0}
///Complex gain real part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REAL_2(nband) (fitsext_column){"REAL_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "Complex gain real part for polarization 2", {""}, 0, 0, 0}
///Complex gain imaginary part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_IMAG_2(nband) (fitsext_column){"IMAG_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "Complex gain imaginary part for polarization 2", {""}, 0, 0, 0}
///Reliability weight of complex gain for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_2(nband) (fitsext_column){"WEIGHT_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "", "", "Reliability weight of complex gain for polarization 2", {""}, 0, 0, 0}
///Reference antenna for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REFANT_2(nband) (fitsext_column){"REFANT_2", FITS_ELEMENT_SIGNED_INT_32_BIT itostr(nband), "", "", "Reference antenna for polarization 2", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI CALIBRATION table
///1
#define FITSIDI_CALIBRATION_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Maximum antenna number in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_ANT (fitsext_column){"NO_ANT", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Maximum antenna number in the table", {""}, 0, 0, 0}
///Number of polarizations in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table", {""}, 0, 0, 0}

///Columns for the FITS-IDI MODEL_COMPS table
///days Central time of interval
#define FITSIDI_MODEL_COMPS_COLUMN_TIME (fitsext_column){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval", {""}, 0, 0, 0}
///Source ID number
#define FITSIDI_MODEL_COMPS_COLUMN_SOURCE_ID (fitsext_column){"SOURCE_ID", "1J", "", "", "Source ID number", {""}, 0, 0, 0}
///Antenna number
#define FITSIDI_MODEL_COMPS_COLUMN_ANTENNA_NO (fitsext_column){"ANTENNA_NO", "1J", "", "", "Antenna number", {""}, 0, 0, 0}
///Array number
#define FITSIDI_MODEL_COMPS_COLUMN_ARRAY (fitsext_column){"ARRAY", "1J", "", "", "Array number", {""}, 0, 0, 0}
///Frequency setup number
#define FITSIDI_MODEL_COMPS_COLUMN_FREQID (fitsext_column){"FREQID", "1J", "", "", "Frequency setup number", {""}, 0, 0, 0}
///sec Atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_ATMOS (fitsext_column){"ATMOS", "1D", FITS_MEASURE_UNIT_SECOND, "", "Atmospheric delay", {""}, 0, 0, 0}
///sec/sec Time derivative of atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_DATMOS (fitsext_column){"DATMOS", "1D", "sec/sec", "", "Time derivative of atmospheric delay", {""}, 0, 0, 0}
///sec Group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GDELAY (fitsext_column){"GDELAY", "1D", FITS_MEASURE_UNIT_SECOND, "", "Group delay", {""}, 0, 0, 0}
///sec/sec Rate of change of group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GRATE (fitsext_column){"GRATE", "1D", "sec/sec", "", "Rate of change of group delay", {""}, 0, 0, 0}
///sec 'Clock' epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_1 (fitsext_column){"CLOCK_1", "1D", FITS_MEASURE_UNIT_SECOND, "", "'Clock' epoch error", {""}, 0, 0, 0}
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_1 (fitsext_column){"DCLOCK_1", "1D", "sec/sec", "", "Time derivative of clock error", {""}, 0, 0, 0}
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_1(nband) (fitsext_column){"LO_OFFSET_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "LO offset", {""}, 0, 0, 0}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_1(nband) (fitsext_column){"DLO_OFFSET_1", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Hz/sec", "", "Time derivative of LO offset", {""}, 0, 0, 0}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_1 (fitsext_column){"DISP_1", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay", {""}, 0, 0, 0}
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_1 (fitsext_column){"DDISP_1", "1E", "sec m−2/sec", "", " Time derivative of dispersive delay", {""}, 0, 0, 0}
///sec 'Clock' epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_2 (fitsext_column){"CLOCK_2", "1D", FITS_MEASURE_UNIT_SECOND, "", "'Clock' epoch error", {""}, 0, 0, 0}
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_2 (fitsext_column){"DCLOCK_2", "1D", "sec/sec", "", "Time derivative of clock error", {""}, 0, 0, 0}
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_2(nband) (fitsext_column){"LO_OFFSET_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), FITS_MEASURE_UNIT_HZ, "", "LO offset", {""}, 0, 0, 0}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_2(nband) (fitsext_column){"DLO_OFFSET_2", FITS_ELEMENT_FLOATING_POINT_32_BIT itostr(nband), "Hz/sec", "", "Time derivative of LO offset", {""}, 0, 0, 0}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_2 (fitsext_column){"DISP_2", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay", {""}, 0, 0, 0}
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_2 (fitsext_column){"DDISP_2", "1E", "sec m−2/sec", "", "Time derivative of dispersive delay", {""}, 0, 0, 0}

///Mandatory keywords for the FITS-IDI MODEL_COMPS table
///1
#define FITSIDI_MODEL_COMPS_KEYWORD_TABREV (fitsext_column){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1", {""}, 0, 0, 0}
///Reference date
#define FITSIDI_MODEL_COMPS_KEYWORD_RDATE (fitsext_column){"RDATE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Reference date", {""}, 0, 0, 0}
///Number of polarizations in the table
#define FITSIDI_MODEL_COMPS_KEYWORD_NO_POL (fitsext_column){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table", {""}, 0, 0, 0}
///FFT size
#define FITSIDI_MODEL_COMPS_KEYWORD_FFT_SIZE (fitsext_column){"FFT_SIZE", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "FFT size", {""}, 0, 0, 0}
///Oversampling factor
#define FITSIDI_MODEL_COMPS_KEYWORD_OVERSAMP (fitsext_column){"OVERSAMP", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Oversampling factor", {""}, 0, 0, 0}
///Zero padding factor
#define FITSIDI_MODEL_COMPS_KEYWORD_ZERO_PAD (fitsext_column){"ZERO_PAD", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Zero padding factor", {""}, 0, 0, 0}
///Tapering function ('HANNING' or 'UNIFORM')
#define FITSIDI_MODEL_COMPS_KEYWORD_TAPER_FN (fitsext_column){"TAPER_FN", FITS_ELEMENT_CHARACTER_STRING, "", "", "Tapering function ('HANNING' or 'UNIFORM')", {""}, 0, 0, 0}

///SDFITS Convention Table
#define FITS_TABLE_SDFITS "SINGLE DISH"

///SDFITS columns
///common FITS usage
#define SDFITS_COLUMN_OBJECT (fitsext_column){"OBJECT", "8A", "", "", "common FITS usage", {""}, 0, 0, 0}
///common FITS keyword
#define SDFITS_COLUMN_TELESCOP (fitsext_column){"TELESCOP", "8A", "", "", "common FITS keyword", {""}, 0, 0, 0}
///resolution may differ from spacing
#define SDFITS_COLUMN_FREQRES (fitsext_column){"FREQRES", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "resolution may differ from spacing", {""}, 0, 0, 0}
///of backend, not one channel
#define SDFITS_COLUMN_BANDWID (fitsext_column){"BANDWID", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "of backend, not one channel", {""}, 0, 0, 0}
///common FITS usage; JD preferable?
#define SDFITS_COLUMN_DATE_OBS (fitsext_column){"DATE-OBS", "8A", "", "", "common FITS usage; JD preferable?", {""}, 0, 0, 0}
///UT time of day; UT seconds since Oh UT
#define SDFITS_COLUMN_TIME (fitsext_column){"TIME", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "UT time of day; UT seconds since Oh UT", {""}, 0, 0, 0}
///effective integration time
#define SDFITS_COLUMN_EXPOSURE (fitsext_column){"EXPOSURE", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "effective integration time", {""}, 0, 0, 0}
///system, not receiver, temperature
#define SDFITS_COLUMN_TSYS (fitsext_column){"TSYS", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "system, not receiver, temperature", {""}, 0, 0, 0}
///Data buffer
#define SDFITS_COLUMN_DATA (fitsext_column){"DATA", "", "", "", "", {""}, 0, 0, 0}

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

///TEMPSCAL
#define TEMPSCAL_TB "TB"
#define TEMPSCAL_TA "TA"
#define TEMPSCAL_TA_TR "TA*TR"
#define TEMPSCAL_TR "TR*"

///TRX, as for TSYS
///Single Side Band
#define TSYS_SSB "SSB"
///Double Side Band
#define TSYS_DSB "DSB"

///VELDEF
#define VELDEF_RADI "*RADI"
#define VELDEF_OPTI "OPTI"
#define VELDEF_RELA "RELA"
#define VELDEF_LSR "LSR"
#define VELDEF_HELO "HELO"
#define VELDEF_EART "EART"
#define VELDEF_BARI "BARI"
#define VELDEF_OBS "-OBS"
///NMATRIX shall be present with the value 1
#define SDFITS_KEYWORD_NMATRIX (fitsext_column){"NMATRIX", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "1", "NMATRIX shall be present with the value 1", {""}, 0, 0, 0}
///M = number axes in regular matrix, Number pixels on axis m = 1 to M
#define SDFITS_KEYWORD_MAXIS(m) (fitsext_column){"MAXIS" itostr(m), FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "M = number axes in regular matrix, Number pixels on axis m = 1 to M", {""}, 0, 0, 0}
///Set to 'T' — column n contains the data matrix
#define SDFITS_KEYWORD_TMATX(n) (fitsext_column){"TMATX" itostr(n), FITS_ELEMENT_CHARACTER_STRING, "", "T", "Set to 'T'", {""}, 0, 0, 0}
#define SDFITS_KEYWORD_TTYPE(n) (fitsext_column){"TTYPE" itostr(n),"8A", "", "", "Set to ''", {""}, 0, 0, 0}
#define SDFITS_KEYWORD_TFORM(n) (fitsext_column){"TFORM" itostr(n),"8A", "", "", "", {""}, 0, 0, 0}
///shall have the value '' or ''
#define SDFITS_KEYWORD_TUNIT(n) (fitsext_column){"TUNIT" itostr(n), "8A", "", "", "shall have the value 'JY' or 'UNCALIB'", {""}, 0, 0, 0}
///Size in pixels of data buffer
#define SDFITS_KEYWORD_TDIM(n) (fitsext_column){"TDIM" itostr(n), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Size in pixels of data buffer", {""}, 0, 0, 0}
///Name of regular axis m = 1 to M
#define SDFITS_KEYWORD_CTYPE(m) (fitsext_column){"CTYPE" itostr(m), FITS_ELEMENT_CHARACTER_STRING, "", "", "Name of regular axis m = 1 to M", {""}, 0, 0, 0}
///Coordinate increment on axis m = 1 to M
#define SDFITS_KEYWORD_CDELT(m) (fitsext_column){"CDELT" itostr(m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate increment on axis m = 1 to M", {""}, 0, 0, 0}
///Reference pixel on axis m = 1 to M
#define SDFITS_KEYWORD_CRPIX(m) (fitsext_column){"CRPIX" itostr(m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Reference pixel on axis m = 1 to M", {""}, 0, 0, 0}
///Coordinate value at reference pixel on axis m = 1 to M
#define SDFITS_KEYWORD_CRVAL(m) (fitsext_column){"CRVAL" itostr(m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate value at reference pixel on axis m = 1 to M", {""}, 0, 0, 0}

///SDFITS columns
///Observer name
#define SDFITS_COLUMN_OBSERVER (fitsext_column){"OBSERVER", "8A", "", "", "Observer name", {""}, 0, 0, 0}
///Observer & operator initials;
#define SDFITS_COLUMN_OBSID (fitsext_column){"OBSID", "8A", "", "", "Observer & operator initials", {""}, 0, 0, 0}
///Project ID;
#define SDFITS_COLUMN_PROJID (fitsext_column){"PROJID", "8A", "", "", "Project ID", {""}, 0, 0, 0}
///Scan number
#define SDFITS_COLUMN_SCAN (fitsext_column){"SCAN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Scan number", {""}, 0, 0, 0}
///Type of data, observing mode;
#define SDFITS_COLUMN_OBSMODE (fitsext_column){"OBSMODE", "8A", "", "", "Type of data, observing mode", {""}, 0, 0, 0}
///Molecule observed or detected;
#define SDFITS_COLUMN_MOLECULE (fitsext_column){"MOLECULE", "8A", "", "", "Molecule observed or detected", {""}, 0, 0, 0}
///As appropriate;
#define SDFITS_COLUMN_TRANSITI (fitsext_column){"TRANSITI", "8A", "", "", "As appropriate", {""}, 0, 0, 0}
///Normalization of TA;
#define SDFITS_COLUMN_TEMPSCAL (fitsext_column){"TEMPSCAL", "8A", "", "", "Normalization of TA", {""}, 0, 0, 0}
///
#define SDFITS_COLUMN_FRONTEND (fitsext_column){"FRONTEND", "8A", "", "", "", {""}, 0, 0, 0}
///Calibration Temp (K)
#define SDFITS_COLUMN_TCAL (fitsext_column){"TCAL", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Calibration Temp", {""}, 0, 0, 0}
///Hot load temperature (K)
#define SDFITS_COLUMN_THOT (fitsext_column){"THOT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Hot load temperature", {""}, 0, 0, 0}
///Cold load temperature (K)
#define SDFITS_COLUMN_TCOLD (fitsext_column){"TCOLD", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Cold load temperature", {""}, 0, 0, 0}
///Receiver Temp (K), Float
#define SDFITS_COLUMN_TRX (fitsext_column){"TRX", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Receiver Temp, Float", {""}, 0, 0, 0}
///Velocity definition & frame;
#define SDFITS_COLUMN_VELDEF (fitsext_column){"VELDEF", "8A", "", "", "Velocity definition & frame", {""}, 0, 0, 0}
///radial velocity correction; Vref - Vtel
#define SDFITS_COLUMN_VCORR (fitsext_column){"VCORR", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "radial velocity correction; Vref - Vtel", {""}, 0, 0, 0}
///Observed Frequency (Hz)
#define SDFITS_COLUMN_OBSFREQ (fitsext_column){"OBSFREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "Observed Frequency", {""}, 0, 0, 0}
///Image sideband freq (Hz)
#define SDFITS_COLUMN_IMAGFREQ (fitsext_column){"IMAGFREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "Image sideband freq", {""}, 0, 0, 0}
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST (fitsext_column){"LST", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "LST at start of scan", {""}, 0, 0, 0}
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST (fitsext_column){"LST", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "LST at start of scan", {""}, 0, 0, 0}
///Commanded Azimuth (Degrees)
#define SDFITS_COLUMN_AZIMUTH (fitsext_column){"AZIMUTH", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Commanded Azimuth", {""}, 0, 0, 0}
///Commanded Elevation (Degrees)
#define SDFITS_COLUMN_ELEVATIO (fitsext_column){"ELEVATIO", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Commanded Elevation", {""}, 0, 0, 0}
///Opacity at signal freq
#define SDFITS_COLUMN_TAU (fitsext_column){"TAU", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Opacity at signal freq", {""}, 0, 0, 0}
///Opacity at image freq
#define SDFITS_COLUMN_TAUIMAGE (fitsext_column){"TAUIMAGE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Opacity at image freq", {""}, 0, 0, 0}
///Opacity per unit air mass
#define SDFITS_COLUMN_TAUZENIT (fitsext_column){"TAUZENIT", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Opacity per unit air mass", {""}, 0, 0, 0}
///Decimal fraction 0..1
#define SDFITS_COLUMN_HUMIDITY (fitsext_column){"HUMIDITY", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Decimal fraction 0..1", {""}, 0, 0, 0}
///Ambient Temp (K)
#define SDFITS_COLUMN_TAMBIENT (fitsext_column){"TAMBIENT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Ambient Temp (K)", {""}, 0, 0, 0}
///Barometer reading mm Hg
#define SDFITS_COLUMN_PRESSURE (fitsext_column){"PRESSURE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "mm Hg", "", "Barometer reading ", {""}, 0, 0, 0}
///Dew point (K)
#define SDFITS_COLUMN_DEWPOINT (fitsext_column){"DEWPOINT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Dew point", {""}, 0, 0, 0}
///Wind speed m/s
#define SDFITS_COLUMN_WINDSPEE (fitsext_column){"WINDSPEE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "m/s", "", "Wind speed", {""}, 0, 0, 0}
///Degrees West of North
#define SDFITS_COLUMN_WINDDIRE (fitsext_column){"WINDDIRE", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Degrees West of North", {""}, 0, 0, 0}
///Main-beam efficiency
#define SDFITS_COLUMN_BEAMEFF (fitsext_column){"BEAMEFF", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Main-beam efficiency", {""}, 0, 0, 0}
///Antenna Aperature Efficiency
#define SDFITS_COLUMN_APEREFF (fitsext_column){"APEREFF", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Antenna Aperature Efficiency", {""}, 0, 0, 0}
///Rear spillover
#define SDFITS_COLUMN_ETAL (fitsext_column){"ETAL", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Rear spillover", {""}, 0, 0, 0}
///Accounts for forward loss
#define SDFITS_COLUMN_ETAFSS (fitsext_column){"ETAFSS", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Accounts for forward loss", {""}, 0, 0, 0}
///K per Jy
#define SDFITS_COLUMN_ANTGAIN (fitsext_column){"ANTGAIN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "K/Jy", "", "", {""}, 0, 0, 0}
///Large main-beam FWHM
#define SDFITS_COLUMN_BMAJ (fitsext_column){"BMAJ", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Large main-beam FWHM", {""}, 0, 0, 0}
///Small main-beam FWHM
#define SDFITS_COLUMN_BMIN (fitsext_column){"BMIN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Small main-beam FWHM", {""}, 0, 0, 0}
///Beam position angle, measured East of North
#define SDFITS_COLUMN_BPA (fitsext_column){"BPA", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Beam position angle", {""}, 0, 0, 0}
///Site longitude (Degrees)
#define SDFITS_COLUMN_SITELONG (fitsext_column){"SITELONG", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Site longitude", {""}, 0, 0, 0}
///Site latitude (Degrees)
#define SDFITS_COLUMN_SITELAT (fitsext_column){"SITELAT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Site latitude", {""}, 0, 0, 0}
///site elevation in meters
#define SDFITS_COLUMN_SITEELEV (fitsext_column){"SITEELEV", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_METER, "", "site elevation", {""}, 0, 0, 0}
///Epoch of observation (year)
#define SDFITS_COLUMN_EPOCH (fitsext_column){"EPOCH", FITS_ELEMENT_FLOATING_POINT_64_BIT, "year", "", "Epoch of observation", {""}, 0, 0, 0}
///Equinox of coords (year)
#define SDFITS_COLUMN_EQUINOX (fitsext_column){"EQUINOX", FITS_ELEMENT_FLOATING_POINT_64_BIT, "year", "", "Equinox of coords", {""}, 0, 0, 0}

#define SDFITS_TABLE_MAIN (fitsext_column[]){\
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
#define SDFITS_KEYWORD_TELESCOP (fitsext_column){"TELESCOP", "8A", "", "", "", {""}, 0, 0, 0}
///Name of observer.
#define SDFITS_KEYWORD_OBSERVER (fitsext_column){"OBSERVER", "8A", "", "", "", {""}, 0, 0, 0}
///UT date of observation (dd/mm/yy) .
#define SDFITS_KEYWORD_DATE_OBS (fitsext_column){"DATE-OBS", "8A", "", "", "", {""}, 0, 0, 0}
///Max spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMAX (fitsext_column){"DATAMAX", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "", {""}, 0, 0, 0}
///Min spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMIN (fitsext_column){"DATAMIN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "", {""}, 0, 0, 0}

#define FITS_KEYWORD_EXTEND (fitsext_column){"EXTEND", "A", "", "T", "", {""}, 0, 0, 0}

void extfits_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status);
long extfits_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows);
int extfits_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, unsigned long num_rows);
int extfits_append_fits_col(fitsfile *fptr, char* name, char* format);
void extfits_delete_fits_col(fitsfile *fptr, char* name);
int extfits_get_element_size(int typecode);
int extfits_check_column(fitsfile *fptr, char* column, char **expected, int expected_n, int rown);
fitsfile* extfits_create_fits_(size_t *size, void **buf);
int extfits_close_fits(fitsfile *fptr);
dsp_stream_p *extfits_read_sdfits(char *filename, long *nstreams);
dsp_stream_p extfits_read_fits(char *filename);

#ifdef __cplusplus
}
#endif

#endif //_FITS_EXTENSIONS_H
