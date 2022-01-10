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
///Returns non-zero decimal conversion of integer into string with a prefix
inline const char* itostr(const char *prefix, int n)
{
    if(n > 0)
    {
        char *buf = (char*)malloc(strlen(prefix) + log(n) / log(10));
        sprintf(buf, "%s%d", prefix, n);
        return (const char*)buf;
    }
    return prefix;
}

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
#define FITSIDI_COLUMN_CORRELAT (char*[]){"CORRELAT", FITS_ELEMENT_CHARACTER_STRING, "", "", "Name/type of correlator"}
///Version number of the correlator software that produced the file
#define FITSIDI_COLUMN_FXCORVER (char*[]){"FXCORVER", FITS_ELEMENT_CHARACTER_STRING, "", "", "Version number of the correlator software that produced the file"}

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
#define FITSIDI_UV_DATA_AXIS_COMPLEX (char*[]){"COMPLEX", "", "", "", "Real, imaginary, weight"}
///Stokes parameter
#define FITSIDI_UV_DATA_AXIS_STOKES (char*[]){"STOKES", "", "", "", "Stokes parameter"}
///Frequency (spectral channel)
#define FITSIDI_UV_DATA_AXIS_FREQ (char*[]){"FREQ", "", "", "", "Frequency (spectral channel)"}
///Band number
#define FITSIDI_UV_DATA_AXIS_BAND (char*[]){"BAND", "", "", "", "Band number"}
///Right ascension of the phase center
#define FITSIDI_UV_DATA_AXIS_RA (char*[]){"RA", "", "", "", "Right ascension of the phase center"}
///Declination of the phase center
#define FITSIDI_UV_DATA_AXIS_DEC (char*[]){"DEC", "", "", "", "Declination of the phase center"}

///Random parameters for the FITS-IDI UV_DATA table
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU (char*[]){"UU", "1D", FITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-SIN system)"}
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV (char*[]){"VV", "1D", FITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-SIN system)"}
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW (char*[]){"WW", "1D", FITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-SIN system)"}
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU_SIN (char*[]){"UU---SIN", "1D", FITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-SIN system)"}
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV_SIN (char*[]){"VV---SIN", "1D", FITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-SIN system)"}
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW_SIN (char*[]){"WW---SIN", "1D", FITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-SIN system)"}
///seconds u baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_UU_NCP (char*[]){"UU---NCP", "1D", FITS_MEASURE_UNIT_SECOND, "", "u baseline coordinate (-NCP system)"}
///seconds v baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_VV_NCP (char*[]){"VV---NCP", "1D", FITS_MEASURE_UNIT_SECOND, "", "v baseline coordinate (-NCP system)"}
///seconds w baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_WW_NCP (char*[]){"WW---NCP", "1D", FITS_MEASURE_UNIT_SECOND, "", "w baseline coordinate (-NCP system)"}
///days Julian date at 0 hours
#define FITSIDI_UV_DATA_COLUMN_DATE (char*[]){"DATE", "1D", FITS_MEASURE_UNIT_DAY, "", "Julian date at 0 hours"}
///days Time elapsed since 0 hours
#define FITSIDI_UV_DATA_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Time elapsed since 0 hours"}
///Baseline number
#define FITSIDI_UV_DATA_COLUMN_BASELINE (char*[]){"BASELINE", "1J", "", "", "Baseline number"}
///Array number
#define FITSIDI_UV_DATA_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Source ID number
#define FITSIDI_UV_DATA_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Frequency setup ID number
#define FITSIDI_UV_DATA_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup ID number"}
///seconds Integration time
#define FITSIDI_UV_DATA_COLUMN_INTTIM (char*[]){"INTTIM", "1D", FITS_MEASURE_UNIT_SECOND, "", "Integration time"}
///Weights
#define FITSIDI_UV_DATA_COLUMN_WEIGHT(nstokes, nband) (char*[]){"WEIGHT", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nstokes)",", nband), "", "", "Weights"}

///Mandatory keywords for the FITS-IDI UV_DATA table
///2
#define FITSIDI_UV_DATA_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2"}

///NMATRIX shall be present with the value 1
#define FITSIDI_UV_DATA_KEYWORD_NMATRIX (char*[]){"NMATRIX", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "NMATRIX shall be present with the value 1"}
///M = number axes in regular matrix, Number pixels on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_MAXIS(m) {itostr("MAXIS", m), FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "M = number axes in regular matrix, Number pixels on axis m = 1 to M"}
///Set to 'T' — column n contains the visibility matrix
#define FITSIDI_UV_DATA_KEYWORD_TMATX(n) {itostr("TMATX", n), FITS_ELEMENT_CHARACTER_STRING, "", "", "Set to 'T'"}
///Set to 'FLUX'
#define FITSIDI_UV_DATA_KEYWORD_TTYPE(n) {itostr("TTYPE", n), "8A", "", "FLUX", "Set to 'FLUX'"}
///shall have the value 'JY' or 'UNCALIB'
#define FITSIDI_UV_DATA_KEYWORD_TUNIT(n) {itostr("TUNIT", n), "8A", "", "UNCALIB", "Shall have the value 'JY' or 'UNCALIB'"}
///Name of regular axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CTYPE(m) {itostr("CTYPE", m), FITS_ELEMENT_CHARACTER_STRING, "", "", "Name of regular axis m = 1 to M"}
///Coordinate increment on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CDELT(m) {itostr("CDELT", m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate increment on axis m = 1 to M"}
///Reference pixel on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CRPIX(m) {itostr("CRPIX", m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Reference pixel on axis m = 1 to M"}
///Coordinate value at reference pixel on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CRVAL(m) {itostr("CRVAL", m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate value at reference pixel on axis m = 1 to M"}
///The number of Stokes parameters
#define FITSIDI_UV_DATA_KEYWORD_NO_STKD (char*[]){"NO_STKD", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The number of Stokes parameters"}
///The first Stokes parameter coordinate value
#define FITSIDI_UV_DATA_KEYWORD_STK_1 (char*[]){"STK_1", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The first Stokes parameter coordinate value"}
///The number of bands
#define FITSIDI_UV_DATA_KEYWORD_NO_BAND (char*[]){"NO_BAND", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The number of bands"}
///The number of spectral channels per band
#define FITSIDI_UV_DATA_KEYWORD_NO_CHAN (char*[]){"NO_CHAN", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "The number of spectral channels per band"}
///The file reference frequency in Hz
#define FITSIDI_UV_DATA_KEYWORD_REF_FREQ (char*[]){"REF_FREQ", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "The file reference frequency in Hz"}
///The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1
#define FITSIDI_UV_DATA_KEYWORD_CHAN_BW (char*[]){"CHAN_BW", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1"}
///The reference pixel for the frequency axis
#define FITSIDI_UV_DATA_KEYWORD_REF_PIXL (char*[]){"REF_PIXL", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "The reference pixel for the frequency axis"}
///Mean equinox
#define FITSIDI_UV_DATA_KEYWORD_EQUINOX (char*[]){"EQUINOX", "8A", "", "", "Mean equinox"}
///Type of data weights
#define FITSIDI_UV_DATA_KEYWORD_WEIGHTYP (char*[]){"WEIGHTYP", "8A", "", "", "Type of data weights"}

///Columns for the FITS-IDI ARRAY_GEOMETRY table
///Antenna name
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ANNAME (char*[]){"ANNAME", "8A", "", "", "Antenna name"}
///meters Antenna station coordinates (x, y, z)
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STABXYZ (char*[]){"STABXYZ", "3D", FITS_MEASURE_UNIT_METER, "", "Antenna station coordinates (x, y, z)"}
///meters/s First-order derivatives of the station coordinates with respect to time
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DERXYZ (char*[]){"DERXYZ", "3E", "meters/s", "", "First-order derivatives of the station coordinates with respect to time"}
///Orbital parameters
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ORBPARM(norb) (char*[]){"ORBPARM", itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, norb), "", "", "Orbital parameters"}
///Antenna number
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_NOSTA (char*[]){"NOSTA", "1I", "", "", "Antenna number"}
///Mount type
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_MNTSTA (char*[]){"MNTSTA", "1J", "", "", "Mount type"}
///meters Axis offset
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STAXOF (char*[]){"STAXOF", "3E", FITS_MEASURE_UNIT_METER, "", "Axis offset"}
///meters Antenna diameter
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DIAMETER (char*[]){"DIAMETER", "1E", FITS_MEASURE_UNIT_METER, "", "Antenna diameter"}

///Mandatory keywords for the FITS-IDI ARRAY_GEOMETRY table
///1
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Array number
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_EXTVER (char*[]){"EXTVER", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Array number"}
///Array name
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRNAM (char*[]){"ARRNAM", FITS_ELEMENT_CHARACTER_STRING, "", "", "Array name"}
///Coordinate frame
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FRAME (char*[]){"FRAME", FITS_ELEMENT_CHARACTER_STRING, "", "", "Coordinate frame"}
///x coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYX (char*[]){"ARRAYX", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "x coordinate of array center (m)"}
///y coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYY (char*[]){"ARRAYY", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "y coordinate of array center (m)"}
///z coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYZ (char*[]){"ARRAYZ", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "z coordinate of array center (m)"}
///norb= number orbital parameters in table
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_NUMORB (char*[]){"NUMORB", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "norb= number orbital parameters in table"}
///Reference frequency (Hz)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FREQ (char*[]){"FREQ", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Reference frequency (Hz)"}
///Time system
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TIMESYS (char*[]){"TIMESYS", FITS_ELEMENT_CHARACTER_STRING, "", "", "Time system"}
///Reference date
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_RDATE (char*[]){"RDATE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Reference date"}
///GST at 0h on reference date (degrees)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_GSTIA0 (char*[]){"GSTIA0", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "GST at 0h on reference date (degrees)"}
///Earth's rotation rate (degrees/day)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_DEGPDY (char*[]){"DEGPDY", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Earth's rotation rate (degrees/day)"}
///UT1 - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_UT1UTC (char*[]){"UT1UTC", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "UT1 - UTC (sec)"}
///IAT - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_IATUTC (char*[]){"IATUTC", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "IAT - UTC (sec)"}
///x coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARX (char*[]){"POLARX", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "x coordinate of North Pole (arc seconds)"}
///y coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARY (char*[]){"POLARY", FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "y coordinate of North Pole (arc seconds)"}

///Columns for the FITS-IDI ANTENNA table
///days Central time of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of period covered by record"}
///days Duration of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of period covered by record"}
///Antenna name
#define FITSIDI_ANTENNA_COLUMN_ANNAME (char*[]){"ANNAME", "8A", "", "", "Antenna name"}
///Antenna number
#define FITSIDI_ANTENNA_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_ANTENNA_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_ANTENNA_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Number of digitizer levels
#define FITSIDI_ANTENNA_COLUMN_NO_LEVELS (char*[]){"NO_LEVELS", "1J", "", "", "Number of digitizer levels"}
///Feed A polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYA (char*[]){"POLTYA", "1A", "Feed A polarization label"}
///degrees Feed A orientation
#define FITSIDI_ANTENNA_COLUMN_POLAA(nband) (char*[]){"POLAA", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_DEGREE, "", "Feed A orientation"}
///Feed A polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALA(npcal, nband) (char*[]){"POLCALA", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, npcal)",", nband), "", "", "Feed A polarization parameters"}
///Feed B polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYB (char*[]){"POLTYB", "1A", "Feed B polarization label"}
///degrees Feed B orientation
#define FITSIDI_ANTENNA_COLUMN_POLAB(nband) (char*[]){"POLAB", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_DEGREE, "", "Feed B orientation"}
///Feed B polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALB(npcal, nband) (char*[]){"POLCALB", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, npcal)",", nband), "", "", "Feed B polarization parameters"}
///degrees / m Antenna beam fwhm
#define FITSIDI_ANTENNA_COLUMN_BEAMFWHM(nband) (char*[]){"BEAMFWHM", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_DEGREE, "", "/ m Antenna beam fwhm"}

///Polarization parameters
///Linear approximation for circular feeds
#define FITSIDI_ANTENNA_POLPARM_APPROX "APPROX"
///Linear approximation for linear feeds
#define FITSIDI_ANTENNA_POLPARM_LIN "X-Y LIN"
///Orientation and ellipticity
#define FITSIDI_ANTENNA_POLPARM_ORI_ELP "ORI-ELP"

///Mandatory keywords for the FITS-IDI ANTENNA table
///1
#define FITSIDI_ANTENNA_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///npcal = 0 or 2, number of polarization calibration constants
#define FITSIDI_ANTENNA_KEYWORD_NOPCAL (char*[]){"NOPCAL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "npcal = 0 or 2, number of polarization calibration constants"}
///The feed polarization parameterization
#define FITSIDI_ANTENNA_KEYWORD_POLTYPE (char*[]){"POLTYPE", FITS_ELEMENT_CHARACTER_STRING, "", "", "The feed polarization parameterization"}

///Columns for the FITS-IDI FREQUENCY table
///Frequency setup number
#define FITSIDI_FREQUENCY_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Hz Frequency offsets
#define FITSIDI_FREQUENCY_COLUMN_BANDFREQ(nband) (char*[]){"BANDFREQ", itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Frequency offsets"}
///Hz Individual channel widths
#define FITSIDI_FREQUENCY_COLUMN_CH_WIDTH(nband) (char*[]){"CH_WIDTH", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Individual channel widths"}
///Hz Total bandwidths of bands
#define FITSIDI_FREQUENCY_COLUMN_TOTAL_BANDWIDTH(nband) (char*[]){"TOTAL_BANDWIDTH", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Total bandwidths of bands"}
///Sideband flag
#define FITSIDI_FREQUENCY_COLUMN_SIDEBAND(nband) (char*[]){"SIDEBAND", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Sideband flag"}

///Mandatory keywords for the FITS-IDI FREQUENCY table
///1
#define FITSIDI_FREQUENCY_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}

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
#define FITSIDI_SOURCE_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Source name
#define FITSIDI_SOURCE_COLUMN_SOURCE (char*[]){"SOURCE", "16A", "", "", "Source name"}
///Source name numeric qualifier
#define FITSIDI_SOURCE_COLUMN_QUAL (char*[]){"QUAL", "1J", "", "", "Source name numeric qualifier"}
///Calibrator code
#define FITSIDI_SOURCE_COLUMN_CALCODE (char*[]){"CALCODE", "4A", "", "", "Calibrator code"}
///Frequency setup number
#define FITSIDI_SOURCE_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Jy Stokes I flux density
#define FITSIDI_SOURCE_COLUMN_IFLUX(nband) (char*[]){"IFLUX", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Jy", "", "Stokes I flux density"}
///Jy Stokes Q flux density
#define FITSIDI_SOURCE_COLUMN_QFLUX(nband) (char*[]){"QFLUX", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Jy", "", "Stokes Q flux density"}
///Jy Stokes U flux density
#define FITSIDI_SOURCE_COLUMN_UFLUX(nband) (char*[]){"UFLUX", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Jy", "", "Stokes U flux density"}
///Jy Stokes V flux density
#define FITSIDI_SOURCE_COLUMN_VFLUX(nband) (char*[]){"VFLUX", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Jy", "", "Stokes V flux density"}
///Jy Spectral index for each band
#define FITSIDI_SOURCE_COLUMN_ALPHA(nband) (char*[]){"ALPHA", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Jy", "", "Spectral index for each band"}
///Hz Frequency offset for each band
#define FITSIDI_SOURCE_COLUMN_FREQOFF(nband) (char*[]){"FREQOFF", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Frequency offset for each band"}
///degrees Right ascension at mean equinox
#define FITSIDI_SOURCE_COLUMN_RAEPO (char*[]){"RAEPO", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Right ascension at mean equinox"}
///degrees Declination at mean equinox
#define FITSIDI_SOURCE_COLUMN_DECEPO (char*[]){"DECEPO", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Declination at mean equinox"}
///Mean equinox
#define FITSIDI_SOURCE_COLUMN_EQUINOX (char*[]){"EQUINOX", "8A", "", "", "Mean equinox"}
///degrees Apparent right ascension
#define FITSIDI_SOURCE_COLUMN_RAAPP (char*[]){"RAAPP", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Apparent right ascension"}
///degrees Apparent declination
#define FITSIDI_SOURCE_COLUMN_DECAPP (char*[]){"DECAPP", "1D", FITS_MEASURE_UNIT_DEGREE, "", "Apparent declination"}
///meters/sec Systemic velocity for each band
#define FITSIDI_SOURCE_COLUMN_SYSVEL(nband) (char*[]){"SYSVEL", itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, nband), "meters/sec", "", "Systemic velocity for each band"}
///Velocity type
#define FITSIDI_SOURCE_COLUMN_VELTYP (char*[]){"VELTYP", "8A", "", "", "Velocity type"}
///Velocity definition
#define FITSIDI_SOURCE_COLUMN_VELDEF (char*[]){"VELDEF", "8A", "", "", "Velocity definition"}
///Hz Line rest frequency for each band
#define FITSIDI_SOURCE_COLUMN_RESTFREQ(nband) (char*[]){"RESTFREQ", itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Line rest frequency for each band"}
///degrees/day Proper motion in right ascension
#define FITSIDI_SOURCE_COLUMN_PMRA (char*[]){"PMRA", "1D", "degrees/day", "", "Proper motion in right ascension"}
///degrees/day Proper motion in declination
#define FITSIDI_SOURCE_COLUMN_PMDEC (char*[]){"PMDEC", "1D", "degrees/day", "", "Proper motion in declination"}
///arcseconds Parallax of source
#define FITSIDI_SOURCE_COLUMN_PARALLAX (char*[]){"PARALLAX", "1E", FITS_MEASURE_UNIT_ARCSEC, "", "Parallax of source"}
///years Epoch of observation
#define FITSIDI_SOURCE_COLUMN_EPOCH (char*[]){"EPOCH", "1D", FITS_MEASURE_UNIT_YEAR, "", "Epoch of observation"}

///Mandatory keywords for the FITS-IDI SOURCE table
///1
#define FITSIDI_SOURCE_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}

///Columns for the FITS-IDI INTERFEROMETER_MODEL table
///days Starting time of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Starting time of interval"}
///days Duration of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval"}
///Source ID number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Antenna number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///rad m−2 Ionospheric Faraday rotation
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_I_FAR_ROT (char*[]){"I.FAR.ROT", "1E", "rad m−2", "", "Ionospheric Faraday rotation"}
///Hz Time variable frequency offsets
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQ_VAR(nband) (char*[]){"FREQ.VAR", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Time variable frequency offsets"}
///turns Phase delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_1(npoly , nband) (char*[]){"PDELAY_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), "turns", "", "Phase delay polynomials for polarization 1"}
///seconds Group delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_1(npoly , nband) (char*[]){"GDELAY_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), FITS_MEASURE_UNIT_SECOND, "", "Group delay polynomials for polarization 1"}
///Hz Phase delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_1(npoly , nband) (char*[]){"PRATE_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), FITS_MEASURE_UNIT_HZ, "", "Phase delay rate polynomials for polarization 1"}
///sec/sec Group delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_1(npoly , nband) (char*[]){"GRATE_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), "sec/sec", "", "Group delay rate polynomials for polarization 1"}
///sec m−2 Dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_1 (char*[]){"DISP_1", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay for polarization 1"}
///sec m−2/sec Rate of change of dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_1 (char*[]){"DDISP_1", "1E", "sec m−2/sec", "", " Rate of change of dispersive delay for polarization 1"}
///turns Phase delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_2(npoly , nband) (char*[]){"PDELAY_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), "turns", "", "Phase delay polynomials for polarization 2"}
///seconds Group delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_2(npoly , nband) (char*[]){"GDELAY_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), FITS_MEASURE_UNIT_SECOND, "", "Group delay polynomials for polarization 2"}
///Hz Phase delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_2(npoly , nband) (char*[]){"PRATE_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), FITS_MEASURE_UNIT_HZ, "", "Phase delay rate polynomials for polarization 2"}
///sec/sec Group delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_2(npoly , nband) (char*[]){"GRATE_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, npoly )",", nband), "sec/sec", "", "Group delay rate polynomials for polarization 2"}
///sec m−2 Dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_2 (char*[]){"DISP_2", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay for polarization 2"}
///sec m−2/sec Rate of change of dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_2 (char*[]){"DDISP_2", "1E", "sec m−2/sec", "", " Rate of change of dispersive delay for polarization 2"}

///Mandatory keywords for the FITS-IDI INTERFEROMETER_MODEL table
///2
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2"}
///Number of polynomial terms npoly
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NPOLY (char*[]){"NPOLY", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polynomial terms npoly"}
///Number of polarizations
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations"}

///Columns for the FITS-IDI SYSTEM_TEMPERATURE table
///days Central time of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///days Duration of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval"}
///Source ID number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Antenna number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Kelvin System temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_1(nband) (char*[]){"TSYS_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperatures for polarization 1"}
///Kelvin Antenna temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_1(nband) (char*[]){"TANT_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperatures for polarization 1"}
///Kelvin System temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_2(nband) (char*[]){"TSYS_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperatures for polarization 2"}
///Kelvin Antenna temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_2(nband) (char*[]){"TANT_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperatures for polarization 2"}

///Mandatory keywords for the FITS-IDI SYSTEM_TEMPERATURE table
///1
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Number of polarizations in the table
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table"}

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
#define FITSIDI_GAIN_CURVE_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_GAIN_CURVE_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_GAIN_CURVE_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Gain curve types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_1(nband) (char*[]){"TYPE_1", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Gain curve types for polarization 1"}
///Number of terms or entries for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_1(nband) (char*[]){"NTERM_1", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Number of terms or entries for polarization 1"}
///x value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_1(nband) (char*[]){"X_TYP_1", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "x value types for polarization 1"}
///y value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_1(nband) (char*[]){"Y_TYP_1", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "y value types for polarization 1"}
///x values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_1(nband) (char*[]){"X_VAL_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "x values for polarization 1"}
///y values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_1(ntab, nband) (char*[]){"Y_VAL_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntab)",", nband), "", "", "y values for polarization 1"}
///Relative gain values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_1(ntab, nband) (char*[]){"GAIN_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntab)",", nband), "", "", "Relative gain values for polarization 1"}
///K/Jy Sensitivities for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_1(nband) (char*[]){"SENS_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "K/Jy", "", " Sensitivities for polarization 1"}
///Gain curve types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_2(nband) (char*[]){"TYPE_2", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Gain curve types for polarization 2"}
///Number of terms or entries for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_2(nband) (char*[]){"NTERM_2", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Number of terms or entries for polarization 2"}
///x value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_2(nband) (char*[]){"X_TYP_2", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "x value types for polarization 2"}
///y value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_2(nband) (char*[]){"Y_TYP_2", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "y value types for polarization 2"}
///x values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_2(nband) (char*[]){"X_VAL_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "x values for polarization 2"}
///y values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_2(ntab, nband) (char*[]){"Y_VAL_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntab)",", nband), "", "", "y values for polarization 2"}
///Relative gain values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_2(ntab, nband) (char*[]){"GAIN_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntab)",", nband), "", "", "Relative gain values for polarization 2"}
///K/Jy Sensitivities for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_2(nband) (char*[]){"SENS_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "K/Jy", "", " Sensitivities for polarization 2"}

///Mandatory keywords for the FITS-IDI GAIN_CURVE table
///1
#define FITSIDI_GAIN_CURVE_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Number of polarizations in the table
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table"}
///Number of tabulated values ntab
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_TABS (char*[]){"NO_TABS", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of tabulated values ntab"}

///Columns for the FITS-IDI PHASE-CAL table
///days Central time of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///days Duration of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval"}
///Source ID number
#define FITSIDI_PHASE_CAL_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Antenna number
#define FITSIDI_PHASE_CAL_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_PHASE_CAL_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_PHASE_CAL_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///seconds Cable calibration measurement
#define FITSIDI_PHASE_CAL_COLUMN_CABLE_CAL (char*[]){"CABLE_CAL", "1D", FITS_MEASURE_UNIT_SECOND, "", "Cable calibration measurement"}
///percent State counts for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_STATE_1(nband) (char*[]){"STATE_1", itostr("E4,", nband), FITS_MEASURE_UNIT_PERCENT, "", "State counts for polarization 1"}
///Hz Phase-cal tone frequencies for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_1(ntone, nband) (char*[]){"PC_FREQ_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, ntone)",", nband), FITS_MEASURE_UNIT_HZ, "", "Phase-cal tone frequencies for polarization 1"}
///Real parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_1(ntone, nband) (char*[]){"PC_REAL_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntone)",", nband), "", "", "Real parts of phase-cal measurements for polarization 1"}
///Imaginary parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_1(ntone, nband) (char*[]){"PC_IMAG_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntone)",", nband), "", "", "Imaginary parts of phase-cal measurements for polarization 1"}
///sec/sec Phase-cal rates for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_1(ntone, nband) (char*[]){"PC_RATE_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntone)",", nband), "sec/sec", "", "Phase-cal rates for polarization 1"}
///percent State counts for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_STATE_2(nband) (char*[]){"STATE_2", itostr("E4,", nband), FITS_MEASURE_UNIT_PERCENT, "", "State counts for polarization 2"}
///Hz Phase-cal tone frequencies for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_2(ntone, nband) (char*[]){"PC_FREQ_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, ntone)",", nband), FITS_MEASURE_UNIT_HZ, "", "Phase-cal tone frequencies for polarization 2"}
///Real parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_2(ntone, nband) (char*[]){"PC_REAL_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntone)",", nband), "", "", "Real parts of phase-cal measurements for polarization 2"}
///Imaginary parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_2(ntone, nband) (char*[]){"PC_IMAG_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntone)",", nband), "", "", "Imaginary parts of phase-cal measurements for polarization 2"}
///sec/sec Phase-cal rates for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_2(ntone, nband) (char*[]){"PC_RATE_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, ntone)",", nband), "sec/sec", "", "Phase-cal rates for polarization 2"}

///Mandatory keywords for the FITS-IDI PHASE-CAL table
///2
#define FITSIDI_PHASE_CAL_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2"}
///Number of polarizations in the table
#define FITSIDI_PHASE_CAL_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table"}
///Number of tones ntone
#define FITSIDI_PHASE_CAL_KEYWORD_NO_TABS (char*[]){"NO_TABS", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of tones ntone"}

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
#define FITSIDI_FLAG_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Array number
#define FITSIDI_FLAG_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Antenna numbers
#define FITSIDI_FLAG_COLUMN_ANTS (char*[]){"ANTS", "2J", "", "", "Antenna numbers"}
///Frequency setup number
#define FITSIDI_FLAG_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///days Time range
#define FITSIDI_FLAG_COLUMN_TIMERANG (char*[]){"TIMERANG", "2E", FITS_MEASURE_UNIT_DAY, "", "Time range"}
///Band flags
#define FITSIDI_FLAG_COLUMN_BANDS(nband) (char*[]){"BANDS", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Band flags"}
///Channel range
#define FITSIDI_FLAG_COLUMN_CHANS (char*[]){"CHANS", "2J", "", "", "Channel range"}
///Polarization flags
#define FITSIDI_FLAG_COLUMN_PFLAGS (char*[]){"PFLAGS", "4J", "", "", "Polarization flags"}
///Reason for flag
#define FITSIDI_FLAG_COLUMN_REASON(n) (char*[]){"REASON", itostr("", n)FITS_ELEMENT_CHARACTER_STRING, "", "", "Reason for flag"}
///Severity code
#define FITSIDI_FLAG_COLUMN_SEVERITY (char*[]){"SEVERITY", "1J", "", "", "Severity code"}

///Mandatory keywords for the FITS-IDI FLAG table
///2
#define FITSIDI_FLAG_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2"}

///Columns for the FITS-IDI WEATHER table
///days Central time of interval
#define FITSIDI_WEATHER_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///days Duration of interval
#define FITSIDI_WEATHER_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval"}
///Antenna number
#define FITSIDI_WEATHER_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Centigrade Surface air temperature
#define FITSIDI_WEATHER_COLUMN_TEMPERATURE (char*[]){"TEMPERATURE", "1E", "", "", "Centigrade Surface air temperature"}
///millibar Surface air pressure
#define FITSIDI_WEATHER_COLUMN_PRESSURE (char*[]){"PRESSURE", "1E", "millibar", "", "Surface air pressure"}
///Centigrade Dewpoint temperature
#define FITSIDI_WEATHER_COLUMN_DEWPOINT (char*[]){"DEWPOINT", "1E", "", "", "Centigrade Dewpoint temperature"}
///m s−1 Wind velocity
#define FITSIDI_WEATHER_COLUMN_WIND_VELOCITY (char*[]){"WIND_VELOCITY", "1E", "m s−1", "", " Wind velocity"}
///degrees Wind direction East from North
#define FITSIDI_WEATHER_COLUMN_WIND_DIRECTION (char*[]){"WIND_DIRECTION", "1E", FITS_MEASURE_UNIT_DEGREE, "", "Wind direction East from North"}
///m−2 Water column
#define FITSIDI_WEATHER_COLUMN_WVR_H2O (char*[]){"WVR_H2O", "1E", "m−2", "", "Water column"}
///m−2 Electron column
#define FITSIDI_WEATHER_COLUMN_IONOS_ELECTRON (char*[]){"IONOS_ELECTRON", "1E", "m−2", "", "Electron column"}

///Mandatory keywords for the FITS-IDI WEATHER table
///2
#define FITSIDI_WEATHER_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "2"}
///Reference date
#define FITSIDI_WEATHER_KEYWORD_RDATE (char*[]){"RDATE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Reference date"}

///Columns for the FITS-IDI BASELINE table
///days Central time of interval
#define FITSIDI_BASELINE_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///Source ID number
#define FITSIDI_BASELINE_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Array number
#define FITSIDI_BASELINE_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Antenna numbers forming baseline
#define FITSIDI_BASELINE_COLUMN_ANTENNA_NOS (char*[]){"ANTENNA_NOS.", "2J", "", "", "Antenna numbers forming baseline"}
///Frequency setup number
#define FITSIDI_BASELINE_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Real part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_REAL_M(nstokes, nband) (char*[]){"REAL_M", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nstokes)",", nband), "", "", "Real part of multiplicative correction"}
///Imaginary part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_IMAG_M(nstokes, nband) (char*[]){"IMAG_M", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nstokes)",", nband), "", "", "Imaginary part of multiplicative correction"}
///Real part of additive correction
#define FITSIDI_BASELINE_COLUMN_REAL_A(nstokes, nband) (char*[]){"REAL_A", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nstokes)",", nband), "", "", "Real part of additive correction"}
///Imaginary part of additive correction
#define FITSIDI_BASELINE_COLUMN_IMAG_A(nstokes, nband) (char*[]){"IMAG_A", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nstokes)",", nband), "", "", "Imaginary part of additive correction"}

///Mandatory keywords for the FITS-IDI BASELINE table
///1
#define FITSIDI_BASELINE_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Maximum antenna number in the table
#define FITSIDI_BASELINE_KEYWORD_NO_ANT (char*[]){"NO_ANT", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Maximum antenna number in the table"}

///Columns for the FITS-IDI BANDPASS table
///days Central time of interval
#define FITSIDI_BANDPASS_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///days Duration of interval
#define FITSIDI_BANDPASS_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval"}
///Source ID number
#define FITSIDI_BANDPASS_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Antenna number
#define FITSIDI_BANDPASS_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_BANDPASS_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_BANDPASS_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Hz Channel bandwidth
#define FITSIDI_BANDPASS_COLUMN_BANDWIDTH (char*[]){"BANDWIDTH", "1E", FITS_MEASURE_UNIT_HZ, "", "Channel bandwidth"}
///Hz Frequency of each band
#define FITSIDI_BANDPASS_COLUMN_BAND_FREQ(nband) (char*[]){"BAND_FREQ", itostr(FITS_ELEMENT_FLOATING_POINT_64_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "Frequency of each band"}
///Reference antenna for polarization 1
#define FITSIDI_BANDPASS_COLUMN_REFANT_1 (char*[]){"REFANT_1", "1J", "", "", "Reference antenna for polarization 1"}
///Real part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BREAL_1(nbach, nband) (char*[]){"BREAL_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nbach)",", nband), "", "", "Real part of bandpass correction for polarization 1"}
///Imaginary part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BIMAG_1(nbach, nband) (char*[]){"BIMAG_1", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nbach)",", nband), "", "", "Imaginary part of bandpass correction for polarization 1"}
///Reference antenna for polarization 2
#define FITSIDI_BANDPASS_COLUMN_REFANT_2 (char*[]){"REFANT_2", "1J", "", "", "Reference antenna for polarization 2"}
///Real part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BREAL_2(nbach, nband) (char*[]){"BREAL_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nbach)",", nband), "", "", "Real part of bandpass correction for polarization 2"}
///Imaginary part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BIMAG_2(nbach, nband) (char*[]){"BIMAG_2", itostr(itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nbach)",", nband), "", "", "Imaginary part of bandpass correction for polarization 2"}

///Mandatory keywords for the FITS-IDI BANDPASS table
///1
#define FITSIDI_BANDPASS_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Maximum antenna number in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_ANT (char*[]){"NO_ANT", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Maximum antenna number in the table"}
///Number of polarizations in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table"}
///Number of spectral channels in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_BACH (char*[]){"NO_BACH", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of spectral channels in the table"}
///Data channel number for first channel in the table
#define FITSIDI_BANDPASS_KEYWORD_STRT_CHN (char*[]){"STRT_CHN", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Data channel number for first channel in the table"}

///Columns for the FITS-IDI CALIBRATION table
///days Central time of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///days Duration of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME_INTERVAL (char*[]){"TIME_INTERVAL", "1E", FITS_MEASURE_UNIT_DAY, "", "Duration of interval"}
///Source ID number
#define FITSIDI_CALIBRATION_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Antenna number
#define FITSIDI_CALIBRATION_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_CALIBRATION_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_CALIBRATION_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///Kelvin System temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TSYS_1(nband) (char*[]){"TSYS_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperature for polarization 1"}
///Kelvin Antenna temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TANT_1(nband) (char*[]){"TANT_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperature for polarization 1"}
///Kelvin/Jy Sensitivity at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_1(nband) (char*[]){"SENSITIVITY_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Kelvin/Jy", "", "Sensitivity at polarization 1"}
///radians Phase at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_PHASE_1(nband) (char*[]){"PHASE_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_RAD, "", "Phase at polarization 1"}
///sec/sec Rate of change of delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_RATE_1(nband) (char*[]){"RATE_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "sec/sec", "", "Rate of change of delay of polarization 1"}
///seconds Delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_DELAY_1(nband) (char*[]){"DELAY_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_SECOND, "", "Delay of polarization 1"}
///Complex gain real part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REAL_1(nband) (char*[]){"REAL_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "Complex gain real part for polarization 1"}
///Complex gain imaginary part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_IMAG_1(nband) (char*[]){"IMAG_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "Complex gain imaginary part for polarization 1"}
///Reliability weight of complex gain for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_1(nband) (char*[]){"WEIGHT_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "Reliability weight of complex gain for polarization 1"}
///Reference antenna for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REFANT_1(nband) (char*[]){"REFANT_1", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Reference antenna for polarization 1"}
///Kelvin System temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TSYS_2(nband) (char*[]){"TSYS_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "System temperature for polarization 2"}
///Kelvin Antenna temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TANT_2(nband) (char*[]){"TANT_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_KELVIN, "", "Antenna temperature for polarization 2"}
///Kelvin/Jy Sensitivity at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_2(nband) (char*[]){"SENSITIVITY_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Kelvin/Jy", "", "Sensitivity at polarization 2"}
///radians Phase at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_PHASE_2(nband) (char*[]){"PHASE_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_RAD, "", "Phase at polarization 2"}
///sec/sec Rate of change of delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_RATE_2(nband) (char*[]){"RATE_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "sec/sec", "", "Rate of change of delay of polarization 2"}
///seconds Delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_DELAY_2(nband) (char*[]){"DELAY_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_SECOND, "", "Delay of polarization 2"}
///Complex gain real part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REAL_2(nband) (char*[]){"REAL_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "Complex gain real part for polarization 2"}
///Complex gain imaginary part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_IMAG_2(nband) (char*[]){"IMAG_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "Complex gain imaginary part for polarization 2"}
///Reliability weight of complex gain for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_2(nband) (char*[]){"WEIGHT_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "", "", "Reliability weight of complex gain for polarization 2"}
///Reference antenna for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REFANT_2(nband) (char*[]){"REFANT_2", itostr(FITS_ELEMENT_SIGNED_INT_32_BIT, nband), "", "", "Reference antenna for polarization 2"}

///Mandatory keywords for the FITS-IDI CALIBRATION table
///1
#define FITSIDI_CALIBRATION_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Maximum antenna number in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_ANT (char*[]){"NO_ANT", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Maximum antenna number in the table"}
///Number of polarizations in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table"}

///Columns for the FITS-IDI MODEL_COMPS table
///days Central time of interval
#define FITSIDI_MODEL_COMPS_COLUMN_TIME (char*[]){"TIME", "1D", FITS_MEASURE_UNIT_DAY, "", "Central time of interval"}
///Source ID number
#define FITSIDI_MODEL_COMPS_COLUMN_SOURCE_ID (char*[]){"SOURCE_ID", "1J", "", "", "Source ID number"}
///Antenna number
#define FITSIDI_MODEL_COMPS_COLUMN_ANTENNA_NO (char*[]){"ANTENNA_NO", "1J", "", "", "Antenna number"}
///Array number
#define FITSIDI_MODEL_COMPS_COLUMN_ARRAY (char*[]){"ARRAY", "1J", "", "", "Array number"}
///Frequency setup number
#define FITSIDI_MODEL_COMPS_COLUMN_FREQID (char*[]){"FREQID", "1J", "", "", "Frequency setup number"}
///sec Atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_ATMOS (char*[]){"ATMOS", "1D", FITS_MEASURE_UNIT_SECOND, "", "Atmospheric delay"}
///sec/sec Time derivative of atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_DATMOS (char*[]){"DATMOS", "1D", "sec/sec", "", "Time derivative of atmospheric delay"}
///sec Group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GDELAY (char*[]){"GDELAY", "1D", FITS_MEASURE_UNIT_SECOND, "", "Group delay"}
///sec/sec Rate of change of group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GRATE (char*[]){"GRATE", "1D", "sec/sec", "", "Rate of change of group delay"}
///sec 'Clock' epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_1 (char*[]){"CLOCK_1", "1D", FITS_MEASURE_UNIT_SECOND, "", "'Clock' epoch error"}
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_1 (char*[]){"DCLOCK_1", "1D", "sec/sec", "", "Time derivative of clock error"}
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_1(nband) (char*[]){"LO_OFFSET_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "LO offset"}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_1(nband) (char*[]){"DLO_OFFSET_1", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Hz/sec", "", "Time derivative of LO offset"}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_1 (char*[]){"DISP_1", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay"}
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_1 (char*[]){"DDISP_1", "1E", "sec m−2/sec", "", " Time derivative of dispersive delay"}
///sec 'Clock' epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_2 (char*[]){"CLOCK_2", "1D", FITS_MEASURE_UNIT_SECOND, "", "'Clock' epoch error"}
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_2 (char*[]){"DCLOCK_2", "1D", "sec/sec", "", "Time derivative of clock error"}
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_2(nband) (char*[]){"LO_OFFSET_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), FITS_MEASURE_UNIT_HZ, "", "LO offset"}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_2(nband) (char*[]){"DLO_OFFSET_2", itostr(FITS_ELEMENT_FLOATING_POINT_32_BIT, nband), "Hz/sec", "", "Time derivative of LO offset"}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_2 (char*[]){"DISP_2", "1E", FITS_MEASURE_UNIT_SECOND, "", "Dispersive delay"}
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_2 (char*[]){"DDISP_2", "1E", "sec m−2/sec", "", "Time derivative of dispersive delay"}

///Mandatory keywords for the FITS-IDI MODEL_COMPS table
///1
#define FITSIDI_MODEL_COMPS_KEYWORD_TABREV (char*[]){"TABREV", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "1"}
///Reference date
#define FITSIDI_MODEL_COMPS_KEYWORD_RDATE (char*[]){"RDATE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Reference date"}
///Number of polarizations in the table
#define FITSIDI_MODEL_COMPS_KEYWORD_NO_POL (char*[]){"NO_POL", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Number of polarizations in the table"}
///FFT size
#define FITSIDI_MODEL_COMPS_KEYWORD_FFT_SIZE (char*[]){"FFT_SIZE", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "FFT size"}
///Oversampling factor
#define FITSIDI_MODEL_COMPS_KEYWORD_OVERSAMP (char*[]){"OVERSAMP", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Oversampling factor"}
///Zero padding factor
#define FITSIDI_MODEL_COMPS_KEYWORD_ZERO_PAD (char*[]){"ZERO_PAD", FITS_ELEMENT_SIGNED_INT_16_BIT, "", "", "Zero padding factor"}
///Tapering function ('HANNING' or 'UNIFORM')
#define FITSIDI_MODEL_COMPS_KEYWORD_TAPER_FN (char*[]){"TAPER_FN", FITS_ELEMENT_CHARACTER_STRING, "", "", "Tapering function ('HANNING' or 'UNIFORM')"}

///SDFITS Convention Table
#define FITS_TABLE_SDFITS "SINGLE DISH"

///SDFITS columns
///common FITS usage
#define SDFITS_COLUMN_OBJECT (char*[]){"OBJECT", "8A", "", "", "common FITS usage"}
///common FITS keyword
#define SDFITS_COLUMN_TELESCOP (char*[]){"TELESCOP", "8A", "", "", "common FITS keyword"}
///resolution may differ from spacing
#define SDFITS_COLUMN_FREQRES (char*[]){"FREQRES", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "resolution may differ from spacing"}
///of backend, not one channel
#define SDFITS_COLUMN_BANDWID (char*[]){"BANDWID", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "of backend, not one channel"}
///common FITS usage; JD preferable?
#define SDFITS_COLUMN_DATE_OBS (char*[]){"DATE-OBS", "8A", "", "", "common FITS usage; JD preferable?"}
///UT time of day; UT seconds since Oh UT
#define SDFITS_COLUMN_TIME (char*[]){"TIME", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "UT time of day; UT seconds since Oh UT"}
///effective integration time
#define SDFITS_COLUMN_EXPOSURE (char*[]){"EXPOSURE", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "effective integration time"}
///system, not receiver, temperature
#define SDFITS_COLUMN_TSYS (char*[]){"TSYS", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "system, not receiver, temperature"}
///Data buffer
#define SDFITS_COLUMN_DATA (char*[]){"DATA", "", "", "", ""}

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

///Set to ''
#define SDFITS_KEYWORD_TTYPE(n) {itostr("TTYPE", n),"8A", "", "", "Set to ''"}
///shall have the value '' or ''
#define SDFITS_KEYWORD_TUNIT(n) {itostr("TUNIT", n), "8A", "", "", "shall have the value 'JY' or 'UNCALIB'"}
///Size in pixels of data buffer
#define SDFITS_KEYWORD_TDIM(n) {itostr("TDIM", n), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Size in pixels of data buffer"}
///Name of regular axis m = 1 to M
#define SDFITS_KEYWORD_CTYPE(m) {itostr("CTYPE", m), FITS_ELEMENT_CHARACTER_STRING, "", "", "Name of regular axis m = 1 to M"}
///Coordinate increment on axis m = 1 to M
#define SDFITS_KEYWORD_CDELT(m) {itostr("CDELT", m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate increment on axis m = 1 to M"}
///Reference pixel on axis m = 1 to M
#define SDFITS_KEYWORD_CRPIX(m) {itostr("CRPIX", m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Reference pixel on axis m = 1 to M"}
///Coordinate value at reference pixel on axis m = 1 to M
#define SDFITS_KEYWORD_CRVAL(m) {itostr("CRVAL", m), FITS_ELEMENT_FLOATING_POINT_32_BIT, "", "", "Coordinate value at reference pixel on axis m = 1 to M"}

///SDFITS columns
///Observer name
#define SDFITS_COLUMN_OBSERVER (char*[]){"OBSERVER", "8A", "", "", "Observer name"}
///Observer & operator initials;
#define SDFITS_COLUMN_OBSID (char*[]){"OBSID", "8A", "", "", "Observer & operator initials"}
///Project ID;
#define SDFITS_COLUMN_PROJID (char*[]){"PROJID", "8A", "", "", "Project ID"}
///Scan number
#define SDFITS_COLUMN_SCAN (char*[]){"SCAN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Scan number"}
///Type of data, observing mode;
#define SDFITS_COLUMN_OBSMODE (char*[]){"OBSMODE", "8A", "", "", "Type of data, observing mode"}
///Molecule observed or detected;
#define SDFITS_COLUMN_MOLECULE (char*[]){"MOLECULE", "8A", "", "", "Molecule observed or detected"}
///As appropriate;
#define SDFITS_COLUMN_TRANSITI (char*[]){"TRANSITI", "8A", "", "", "As appropriate"}
///Normalization of TA;
#define SDFITS_COLUMN_TEMPSCAL (char*[]){"TEMPSCAL", "8A", "", "", "Normalization of TA"}
///
#define SDFITS_COLUMN_FRONTEND (char*[]){"FRONTEND", "8A", "", "", ""}
///Calibration Temp (K)
#define SDFITS_COLUMN_TCAL (char*[]){"TCAL", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Calibration Temp"}
///Hot load temperature (K)
#define SDFITS_COLUMN_THOT (char*[]){"THOT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Hot load temperature"}
///Cold load temperature (K)
#define SDFITS_COLUMN_TCOLD (char*[]){"TCOLD", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Cold load temperature"}
///Receiver Temp (K), Float
#define SDFITS_COLUMN_TRX (char*[]){"TRX", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Receiver Temp, Float"}
///Velocity definition & frame;
#define SDFITS_COLUMN_VELDEF (char*[]){"VELDEF", "8A", "", "", "Velocity definition & frame"}
///radial velocity correction; Vref - Vtel
#define SDFITS_COLUMN_VCORR (char*[]){"VCORR", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "radial velocity correction; Vref - Vtel"}
///Observed Frequency (Hz)
#define SDFITS_COLUMN_OBSFREQ (char*[]){"OBSFREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "Observed Frequency"}
///Image sideband freq (Hz)
#define SDFITS_COLUMN_IMAGFREQ (char*[]){"IMAGFREQ", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_HZ, "", "Image sideband freq"}
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST (char*[]){"LST", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "LST at start of scan"}
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST (char*[]){"LST", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_SECOND, "", "LST at start of scan"}
///Commanded Azimuth (Degrees)
#define SDFITS_COLUMN_AZIMUTH (char*[]){"AZIMUTH", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Commanded Azimuth"}
///Commanded Elevation (Degrees)
#define SDFITS_COLUMN_ELEVATIO (char*[]){"ELEVATIO", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Commanded Elevation"}
///Opacity at signal freq
#define SDFITS_COLUMN_TAU (char*[]){"TAU", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Opacity at signal freq"}
///Opacity at image freq
#define SDFITS_COLUMN_TAUIMAGE (char*[]){"TAUIMAGE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Opacity at image freq"}
///Opacity per unit air mass
#define SDFITS_COLUMN_TAUZENIT (char*[]){"TAUZENIT", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Opacity per unit air mass"}
///Decimal fraction 0..1
#define SDFITS_COLUMN_HUMIDITY (char*[]){"HUMIDITY", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Decimal fraction 0..1"}
///Ambient Temp (K)
#define SDFITS_COLUMN_TAMBIENT (char*[]){"TAMBIENT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Ambient Temp (K)"}
///Barometer reading mm Hg
#define SDFITS_COLUMN_PRESSURE (char*[]){"PRESSURE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "mm Hg", "", "Barometer reading "}
///Dew point (K)
#define SDFITS_COLUMN_DEWPOINT (char*[]){"DEWPOINT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_KELVIN, "", "Dew point"}
///Wind speed m/s
#define SDFITS_COLUMN_WINDSPEE (char*[]){"WINDSPEE", FITS_ELEMENT_FLOATING_POINT_64_BIT, "m/s", "", "Wind speed"}
///Degrees West of North
#define SDFITS_COLUMN_WINDDIRE (char*[]){"WINDDIRE", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Degrees West of North"}
///Main-beam efficiency
#define SDFITS_COLUMN_BEAMEFF (char*[]){"BEAMEFF", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Main-beam efficiency"}
///Antenna Aperature Efficiency
#define SDFITS_COLUMN_APEREFF (char*[]){"APEREFF", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Antenna Aperature Efficiency"}
///Rear spillover
#define SDFITS_COLUMN_ETAL (char*[]){"ETAL", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Rear spillover"}
///Accounts for forward loss
#define SDFITS_COLUMN_ETAFSS (char*[]){"ETAFSS", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Accounts for forward loss"}
///K per Jy
#define SDFITS_COLUMN_ANTGAIN (char*[]){"ANTGAIN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "K/Jy", "", ""}
///Large main-beam FWHM
#define SDFITS_COLUMN_BMAJ (char*[]){"BMAJ", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Large main-beam FWHM"}
///Small main-beam FWHM
#define SDFITS_COLUMN_BMIN (char*[]){"BMIN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Small main-beam FWHM"}
///Beam position angle, measured East of North
#define SDFITS_COLUMN_BPA (char*[]){"BPA", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", "Beam position angle"}
///Site longitude (Degrees)
#define SDFITS_COLUMN_SITELONG (char*[]){"SITELONG", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Site longitude"}
///Site latitude (Degrees)
#define SDFITS_COLUMN_SITELAT (char*[]){"SITELAT", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_DEGREE, "", "Site latitude"}
///site elevation in meters
#define SDFITS_COLUMN_SITEELEV (char*[]){"SITEELEV", FITS_ELEMENT_FLOATING_POINT_64_BIT, FITS_MEASURE_UNIT_METER, "", "site elevation"}
///Epoch of observation (year)
#define SDFITS_COLUMN_EPOCH (char*[]){"EPOCH", FITS_ELEMENT_FLOATING_POINT_64_BIT, "year", "", "Epoch of observation"}
///Equinox of coords (year)
#define SDFITS_COLUMN_EQUINOX (char*[]){"EQUINOX", FITS_ELEMENT_FLOATING_POINT_64_BIT, "year", "", "Equinox of coords"}

///Global keywords for the SDFITS SINGLE DISH table
///Designation of Telescope.
#define SDFITS_KEYWORD_TELESCOP (char*[]){"TELESCOP", "8A", "", "", ""}
///Name of observer.
#define SDFITS_KEYWORD_OBSERVER (char*[]){"OBSERVER", "8A", "", "", ""}
///UT date of observation (dd/mm/yy) .
#define SDFITS_KEYWORD_DATE_OBS (char*[]){"DATE-OBS", "8A", "", "", ""}
///Max spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMAX (char*[]){"DATAMAX", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", ""}
///Min spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMIN (char*[]){"DATAMIN", FITS_ELEMENT_FLOATING_POINT_64_BIT, "", "", ""}

#define FITS_KEYWORD_EXTEND (char*[]){"EXTEND", "A", "", "T", ""}

void vlbi_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status);
long vlbi_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows);
int vlbi_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, unsigned long num_rows);
int vlbi_append_fits_col(fitsfile *fptr, char* name, char* format);
void vlbi_delete_fits_col(fitsfile *fptr, char* name);
fitsfile* vlbi_create_fits_(size_t *size, void **buf);
int vlbi_close_fits(fitsfile *fptr);
dsp_stream_p extfits_read_sdfits(char *filename);
dsp_stream_p extfits_read_fits(char *filename);

#ifdef __cplusplus
}
#endif

#endif //_FITS_EXTENSIONS_H
