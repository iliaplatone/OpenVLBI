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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vlbi.h>

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
typedef enum
{
    ///I
    FITS_STOKE_I = 1,
    ///Q
    FITS_STOKE_Q = 2,
    ///U
    FITS_STOKE_U = 3,
    ///V
    FITS_STOKE_V = 4,
    ///RR
    FITS_STOKE_RR = -1,
    ///LL
    FITS_STOKE_LL = -2,
    ///RL
    FITS_STOKE_RL = -3,
    ///LR
    FITS_STOKE_LR = -4,
    ///XX
    FITS_STOKE_XX = -5,
    ///YY
    FITS_STOKE_YY = -6,
    ///XY
    FITS_STOKE_XY = -7,
    ///YX
    FITS_STOKE_YX = -8,
} FITS_STOKE_TYPE;

///FITS element types
typedef enum
{
    FITS_ELEMENT_CHARACTER_STRING = 'A',
    FITS_ELEMENT_LOGICAL = 'L',
    FITS_ELEMENT_BIT = 'X',
    FITS_ELEMENT_UNSIGNED_BYTE = 'B',
    FITS_ELEMENT_SIGNED_BYTE = 'S',
    FITS_ELEMENT_SIGNED_INT_16_BIT = 'I',
    FITS_ELEMENT_UNSIGNED_INT_16_BIT = 'U',
    FITS_ELEMENT_SIGNED_INT_32_BIT = 'J',
    FITS_ELEMENT_UNSIGNED_INT_32_BIT = 'V',
    FITS_ELEMENT_SIGNED_INT_64_BIT = 'K',
    FITS_ELEMENT_FLOATING_POINT_32_BIT = 'E',
    FITS_ELEMENT_FLOATING_POINT_64_BIT = 'D',
    FITS_ELEMENT_COMPLEX_PAIR_32_BIT = 'C',
    FITS_ELEMENT_COMPLEX_PAIR_64_BIT = 'M',
    FITS_ELEMENT_POINTER_128_BIT = 'P',
} FITS_ELEMENT_TYPE;

///FITS Measure units
#define FITS_MEASURE_UNIT_HZ "HZ"
#define FITS_MEASURE_UNIT_SECOND "SECONDS"
#define FITS_MEASURE_UNIT_KELVIN "KELVIN"
#define FITS_MEASURE_UNIT_ARCSEC "ARCSEC"
#define FITS_MEASURE_UNIT_ANGSTROM "ANGSTROM"
#define FITS_MEASURE_UNIT_ARCMIN "ARCMIN"
#define FITS_MEASURE_UNIT_DEGREE "DEGREE"
#define FITS_MEASURE_UNIT_PERCENT "PERCENT"
#define FITS_MEASURE_UNIT_METER "METER"

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
#define FITSIDI_COLUMN_CORRELAT {"CORRELAT", "A", "Name/type of correlator" }
///Version number of the correlator software that produced the file
#define FITSIDI_COLUMN_FXCORVER {"FXCORVER", "A", "Version number of the correlator software that produced the file" }

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
#define FITSIDI_UV_DATA_AXIS_COMPLEX { "COMPLEX", "", "Real, imaginary, weight"}
///Stokes parameter
#define FITSIDI_UV_DATA_AXIS_STOKES { "STOKES", "", "Stokes parameter"}
///Frequency (spectral channel)
#define FITSIDI_UV_DATA_AXIS_FREQ { "FREQ", "", "Frequency (spectral channel)"}
///Band number
#define FITSIDI_UV_DATA_AXIS_BAND { "BAND", "", "Band number"}
///Right ascension of the phase center
#define FITSIDI_UV_DATA_AXIS_RA { "RA", "", "Right ascension of the phase center"}
///Declination of the phase center
#define FITSIDI_UV_DATA_AXIS_DEC { "DEC", "", "Declination of the phase center"}

///Random parameters for the FITS-IDI UV_DATA table
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU {"UU", "1D", "seconds u baseline coordinate (-SIN system)" }
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV {"VV", "1D", "seconds v baseline coordinate (-SIN system)" }
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW {"WW", "1D", "seconds w baseline coordinate (-SIN system)" }
///seconds u baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_UU___SIN {"UU---SIN", "1D", "seconds u baseline coordinate (-SIN system)" }
///seconds v baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_VV___SIN {"VV---SIN", "1D", "seconds v baseline coordinate (-SIN system)" }
///seconds w baseline coordinate (-SIN system)
#define FITSIDI_UV_DATA_COLUMN_WW___SIN {"WW---SIN", "1D", "seconds w baseline coordinate (-SIN system)" }
///seconds u baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_UU___NCP {"UU---NCP", "1D", "seconds u baseline coordinate (-NCP system)" }
///seconds v baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_VV___NCP {"VV---NCP", "1D", "seconds v baseline coordinate (-NCP system)" }
///seconds w baseline coordinate (-NCP system)
#define FITSIDI_UV_DATA_COLUMN_WW___NCP {"WW---NCP", "1D", "seconds w baseline coordinate (-NCP system)" }
///days Julian date at 0 hours
#define FITSIDI_UV_DATA_COLUMN_DATE {"DATE", "1D", "days Julian date at 0 hours" }
///days Time elapsed since 0 hours
#define FITSIDI_UV_DATA_COLUMN_TIME {"TIME", "1D", "days Time elapsed since 0 hours" }
///Baseline number
#define FITSIDI_UV_DATA_COLUMN_BASELINE {"BASELINE", "1J", "Baseline number" }
///Array number
#define FITSIDI_UV_DATA_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Source ID number
#define FITSIDI_UV_DATA_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Frequency setup ID number
#define FITSIDI_UV_DATA_COLUMN_FREQID {"FREQID", "1J", "Frequency setup ID number" }
///seconds Integration time
#define FITSIDI_UV_DATA_COLUMN_INTTIM {"INTTIM", "1D", "seconds Integration time" }
///Weights
#define FITSIDI_UV_DATA_COLUMN_WEIGHT(nstokes, nband) {"WEIGHT", itostr(itostr("E", nstokes)",", nband), "Weights"}

///Mandatory keywords for the FITS-IDI UV_DATA table
///2
#define FITSIDI_UV_DATA_KEYWORD_TABREV {"TABREV", "I", "2" }

///NMATRIX shall be present with the value 1
#define FITSIDI_UV_DATA_KEYWORD_NMATRIX {"NMATRIX", "I", "NMATRIX shall be present with the value 1" }
///M = number axes in regular matrix, Number pixels on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_MAXIS(m) {itostr("MAXIS", m), "I", "M = number axes in regular matrix, Number pixels on axis m = 1 to M" }
///Set to 'FLUX'
#define FITSIDI_UV_DATA_KEYWORD_TTYPE(n) {itostr("TTYPE", n), "8A", "Set to 'FLUX'"}
///Set to 'T' — column n contains the visibility matrix
#define FITSIDI_UV_DATA_KEYWORD_TMATX(n) {itostr("TMATX", n), "A", "Set to 'T'"}
///shall have the value 'JY' or 'UNCALIB'
#define FITSIDI_UV_DATA_KEYWORD_TUNIT(n) {itostr("TUNIT", n), "8A", "shall have the value 'JY' or 'UNCALIB'"}
///Name of regular axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CTYPE(m) {itostr("CTYPE", m), "A", "Name of regular axis m = 1 to M" }
///Coordinate increment on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CDELT(m) {itostr("CDELT", m), "E", "Coordinate increment on axis m = 1 to M" }
///Reference pixel on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CRPIX(m) {itostr("CRPIX", m), "E", "Reference pixel on axis m = 1 to M" }
///Coordinate value at reference pixel on axis m = 1 to M
#define FITSIDI_UV_DATA_KEYWORD_CRVAL(m) {itostr("CRVAL", m), "E", "Coordinate value at reference pixel on axis m = 1 to M" }
///The number of Stokes parameters
#define FITSIDI_UV_DATA_KEYWORD_NO_STKD {"NO_STKD", "I", "The number of Stokes parameters" }
///The first Stokes parameter coordinate value
#define FITSIDI_UV_DATA_KEYWORD_STK_1 {"STK_1", "I", "The first Stokes parameter coordinate value" }
///The number of bands
#define FITSIDI_UV_DATA_KEYWORD_NO_BAND {"NO_BAND", "I", "The number of bands" }
///The number of spectral channels per band
#define FITSIDI_UV_DATA_KEYWORD_NO_CHAN {"NO_CHAN", "I", "The number of spectral channels per band" }
///The file reference frequency in Hz
#define FITSIDI_UV_DATA_KEYWORD_REF_FREQ {"REF_FREQ", "E", "The file reference frequency in Hz" }
///The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1
#define FITSIDI_UV_DATA_KEYWORD_CHAN_BW {"CHAN_BW", "E", "The channel bandwidth in Hz for the first band in the frequency setup with frequency ID number 1" }
///The reference pixel for the frequency axis
#define FITSIDI_UV_DATA_KEYWORD_REF_PIXL {"REF_PIXL", "E", "The reference pixel for the frequency axis" }
///Mean equinox
#define FITSIDI_UV_DATA_KEYWORD_EQUINOX {"EQUINOX", "8A", "Mean equinox" }
///Type of data weights
#define FITSIDI_UV_DATA_KEYWORD_WEIGHTYP {"WEIGHTYP", "8A", "Type of data weights" }

///Columns for the FITS-IDI ARRAY_GEOMETRY table
///Antenna name
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ANNAME {"ANNAME", "8A", "Antenna name" }
///meters Antenna station coordinates (x, y, z)
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STABXYZ {"STABXYZ", "3D", "meters Antenna station coordinates (x, y, z)" }
///meters/s First-order derivatives of the station coordinates with respect to time
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DERXYZ {"DERXYZ", "3E", "meters/s First-order derivatives of the station coordinates with respect to time" }
///Orbital parameters
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_ORBPARM(norb) {"ORBPARM", itostr("D", norb), "Orbital parameters"}
///Antenna number
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_NOSTA {"NOSTA", "1I", "Antenna number" }
///Mount type
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_MNTSTA {"MNTSTA", "1J", "Mount type" }
///meters Axis offset
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_STAXOF {"STAXOF", "3E", "meters Axis offset" }
///meters Antenna diameter
#define FITSIDI_ARRAY_GEOMETRY_COLUMN_DIAMETER {"DIAMETER", "1E", "meters Antenna diameter" }

///Mandatory keywords for the FITS-IDI ARRAY_GEOMETRY table
///1
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TABREV {"TABREV", "I", "1" }
///Array number
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_EXTVER {"EXTVER", "I", "Array number" }
///Array name
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRNAM {"ARRNAM", "A", "Array name" }
///Coordinate frame
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FRAME {"FRAME", "A", "Coordinate frame" }
///x coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYX {"ARRAYX", "E", "x coordinate of array center (m)" }
///y coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYY {"ARRAYY", "E", "y coordinate of array center (m)" }
///z coordinate of array center (m)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_ARRAYZ {"ARRAYZ", "E", "z coordinate of array center (m)" }
///norb= number orbital parameters in table
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_NUMORB {"NUMORB", "I", "norb= number orbital parameters in table" }
///Reference frequency (Hz)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_FREQ {"FREQ", "E", "Reference frequency (Hz)" }
///Time system
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_TIMESYS {"TIMESYS", "A", "Time system" }
///Reference date
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_RDATE {"RDATE", "D", "Reference date" }
///GST at 0h on reference date (degrees)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_GSTIA0 {"GSTIA0", "E", "GST at 0h on reference date (degrees)" }
///Earth's rotation rate (degrees/day)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_DEGPDY {"DEGPDY", "E", "Earth's rotation rate (degrees/day)" }
///UT1 - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_UT1UTC {"UT1UTC", "E", "UT1 - UTC (sec)" }
///IAT - UTC (sec)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_IATUTC {"IATUTC", "E", "IAT - UTC (sec)" }
///x coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARX {"POLARX", "E", "x coordinate of North Pole (arc seconds)" }
///y coordinate of North Pole (arc seconds)
#define FITSIDI_ARRAY_GEOMETRY_KEYWORD_POLARY {"POLARY", "E", "y coordinate of North Pole (arc seconds)" }

///Columns for the FITS-IDI ANTENNA table
///days Central time of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME {"TIME", "1D", "days Central time of period covered by record" }
///days Duration of period covered by record
#define FITSIDI_ANTENNA_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of period covered by record" }
///Antenna name
#define FITSIDI_ANTENNA_COLUMN_ANNAME {"ANNAME", "8A", "Antenna name" }
///Antenna number
#define FITSIDI_ANTENNA_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_ANTENNA_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_ANTENNA_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Number of digitizer levels
#define FITSIDI_ANTENNA_COLUMN_NO_LEVELS {"NO_LEVELS", "1J", "Number of digitizer levels" }
///Feed A polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYA {"POLTYA", "1A", "Feed A polarization label" }
///degrees Feed A orientation
#define FITSIDI_ANTENNA_COLUMN_POLAA(nband) {"POLAA", itostr("E", nband), "degrees Feed A orientation"}
///Feed A polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALA(npcal, nband) {"POLCALA", itostr(itostr("E", npcal)",", nband), "Feed A polarization parameters"}
///Feed B polarization label
#define FITSIDI_ANTENNA_COLUMN_POLTYB {"POLTYB", "1A", "Feed B polarization label" }
///degrees Feed B orientation
#define FITSIDI_ANTENNA_COLUMN_POLAB(nband) {"POLAB", itostr("E", nband), "degrees Feed B orientation"}
///Feed B polarization parameters
#define FITSIDI_ANTENNA_COLUMN_POLCALB(npcal, nband) {"POLCALB", itostr(itostr("E", npcal)",", nband), "Feed B polarization parameters"}
///degrees / m Antenna beam fwhm
#define FITSIDI_ANTENNA_COLUMN_BEAMFWHM(nband) {"BEAMFWHM", itostr("E", nband), "degrees / m Antenna beam fwhm"}

///Polarization parameters
///Linear approximation for circular feeds
#define FITSIDI_ANTENNA_POLPARM_APPROX "APPROX"
///Linear approximation for linear feeds
#define FITSIDI_ANTENNA_POLPARM_LIN "X-Y LIN"
///Orientation and ellipticity
#define FITSIDI_ANTENNA_POLPARM_ORI_ELP "ORI-ELP"

///Mandatory keywords for the FITS-IDI ANTENNA table
///1
#define FITSIDI_ANTENNA_KEYWORD_TABREV {"TABREV", "I", "1" }
///npcal = 0 or 2, number of polarization calibration constants
#define FITSIDI_ANTENNA_KEYWORD_NOPCAL {"NOPCAL", "I", "npcal = 0 or 2, number of polarization calibration constants" }
///The feed polarization parameterization
#define FITSIDI_ANTENNA_KEYWORD_POLTYPE {"POLTYPE", "A", "The feed polarization parameterization" }

///Columns for the FITS-IDI FREQUENCY table
///Frequency setup number
#define FITSIDI_FREQUENCY_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Hz Frequency offsets
#define FITSIDI_FREQUENCY_COLUMN_BANDFREQ(nband) {"BANDFREQ", itostr("D", nband), "Hz Frequency offsets"}
///Hz Individual channel widths
#define FITSIDI_FREQUENCY_COLUMN_CH_WIDTH(nband) {"CH_WIDTH", itostr("E", nband), "Hz Individual channel widths"}
///Hz Total bandwidths of bands
#define FITSIDI_FREQUENCY_COLUMN_TOTAL_BANDWIDTH(nband) {"TOTAL_BANDWIDTH", itostr("E", nband), "Hz Total bandwidths of bands"}
///Sideband flag
#define FITSIDI_FREQUENCY_COLUMN_SIDEBAND(nband) {"SIDEBAND", itostr("J", nband), "Sideband flag"}

///Mandatory keywords for the FITS-IDI FREQUENCY table
///1
#define FITSIDI_FREQUENCY_KEYWORD_TABREV {"TABREV", "I", "1" }

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
#define FITSIDI_SOURCE_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Source name
#define FITSIDI_SOURCE_COLUMN_SOURCE {"SOURCE", "16A", "Source name" }
///Source name numeric qualifier
#define FITSIDI_SOURCE_COLUMN_QUAL {"QUAL", "1J", "Source name numeric qualifier" }
///Calibrator code
#define FITSIDI_SOURCE_COLUMN_CALCODE {"CALCODE", "4A", "Calibrator code" }
///Frequency setup number
#define FITSIDI_SOURCE_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Jy Stokes I flux density
#define FITSIDI_SOURCE_COLUMN_IFLUX(nband) {"IFLUX", itostr("E", nband), "Jy Stokes I flux density"}
///Jy Stokes Q flux density
#define FITSIDI_SOURCE_COLUMN_QFLUX(nband) {"QFLUX", itostr("E", nband), "Jy Stokes Q flux density"}
///Jy Stokes U flux density
#define FITSIDI_SOURCE_COLUMN_UFLUX(nband) {"UFLUX", itostr("E", nband), "Jy Stokes U flux density"}
///Jy Stokes V flux density
#define FITSIDI_SOURCE_COLUMN_VFLUX(nband) {"VFLUX", itostr("E", nband), "Jy Stokes V flux density"}
///Jy Spectral index for each band
#define FITSIDI_SOURCE_COLUMN_ALPHA(nband) {"ALPHA", itostr("E", nband), "Jy Spectral index for each band"}
///Hz Frequency offset for each band
#define FITSIDI_SOURCE_COLUMN_FREQOFF(nband) {"FREQOFF", itostr("E", nband), "Hz Frequency offset for each band"}
///degrees Right ascension at mean equinox
#define FITSIDI_SOURCE_COLUMN_RAEPO {"RAEPO", "1D", "degrees Right ascension at mean equinox" }
///degrees Declination at mean equinox
#define FITSIDI_SOURCE_COLUMN_DECEPO {"DECEPO", "1D", "degrees Declination at mean equinox" }
///Mean equinox
#define FITSIDI_SOURCE_COLUMN_EQUINOX {"EQUINOX", "8A", "Mean equinox" }
///degrees Apparent right ascension
#define FITSIDI_SOURCE_COLUMN_RAAPP {"RAAPP", "1D", "degrees Apparent right ascension" }
///degrees Apparent declination
#define FITSIDI_SOURCE_COLUMN_DECAPP {"DECAPP", "1D", "degrees Apparent declination" }
///meters/sec Systemic velocity for each band
#define FITSIDI_SOURCE_COLUMN_SYSVEL(nband) {"SYSVEL", itostr("D", nband), "meters/sec Systemic velocity for each band"}
///Velocity type
#define FITSIDI_SOURCE_COLUMN_VELTYP {"VELTYP", "8A", "Velocity type" }
///Velocity definition
#define FITSIDI_SOURCE_COLUMN_VELDEF {"VELDEF", "8A", "Velocity definition" }
///Hz Line rest frequency for each band
#define FITSIDI_SOURCE_COLUMN_RESTFREQ(nband) {"RESTFREQ", itostr("D", nband), "Hz Line rest frequency for each band"}
///degrees/day Proper motion in right ascension
#define FITSIDI_SOURCE_COLUMN_PMRA {"PMRA", "1D", "degrees/day Proper motion in right ascension" }
///degrees/day Proper motion in declination
#define FITSIDI_SOURCE_COLUMN_PMDEC {"PMDEC", "1D", "degrees/day Proper motion in declination" }
///arcseconds Parallax of source
#define FITSIDI_SOURCE_COLUMN_PARALLAX {"PARALLAX", "1E", "arcseconds Parallax of source" }
///years Epoch of observation
#define FITSIDI_SOURCE_COLUMN_EPOCH {"EPOCH", "1D", "years Epoch of observation" }

///Mandatory keywords for the FITS-IDI SOURCE table
///1
#define FITSIDI_SOURCE_KEYWORD_TABREV {"TABREV", "I", "1" }

///Columns for the FITS-IDI INTERFEROMETER_MODEL table
///days Starting time of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME {"TIME", "1D", "days Starting time of interval" }
///days Duration of interval
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of interval" }
///Source ID number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Antenna number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///rad m−2 Ionospheric Faraday rotation
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_I_FAR_ROT {"I.FAR.ROT", "1E", "rad m−2 Ionospheric Faraday rotation" }
///Hz Time variable frequency offsets
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_FREQ_VAR(nband) {"FREQ.VAR", itostr("E", nband), "Hz Time variable frequency offsets"}
///turns Phase delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_1(npoly , nband) {"PDELAY_1", itostr(itostr("D", npoly )",", nband), "turns Phase delay polynomials for polarization 1"}
///seconds Group delay polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_1(npoly , nband) {"GDELAY_1", itostr(itostr("D", npoly )",", nband), "seconds Group delay polynomials for polarization 1"}
///Hz Phase delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_1(npoly , nband) {"PRATE_1", itostr(itostr("D", npoly )",", nband), "Hz Phase delay rate polynomials for polarization 1"}
///sec/sec Group delay rate polynomials for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_1(npoly , nband) {"GRATE_1", itostr(itostr("D", npoly )",", nband), "sec/sec Group delay rate polynomials for polarization 1"}
///sec m−2 Dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_1 {"DISP_1", "1E", "sec m−2 Dispersive delay for polarization 1" }
///sec m−2/sec Rate of change of dispersive delay for polarization 1
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_1 {"DDISP_1", "1E", "sec m−2/sec Rate of change of dispersive delay for polarization 1" }
///turns Phase delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PDELAY_2(npoly , nband) {"PDELAY_2", itostr(itostr("D", npoly )",", nband), "turns Phase delay polynomials for polarization 2"}
///seconds Group delay polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GDELAY_2(npoly , nband) {"GDELAY_2", itostr(itostr("D", npoly )",", nband), "seconds Group delay polynomials for polarization 2"}
///Hz Phase delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_PRATE_2(npoly , nband) {"PRATE_2", itostr(itostr("D", npoly )",", nband), "Hz Phase delay rate polynomials for polarization 2"}
///sec/sec Group delay rate polynomials for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_GRATE_2(npoly , nband) {"GRATE_2", itostr(itostr("D", npoly )",", nband), "sec/sec Group delay rate polynomials for polarization 2"}
///sec m−2 Dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DISP_2 {"DISP_2", "1E", "sec m−2 Dispersive delay for polarization 2" }
///sec m−2/sec Rate of change of dispersive delay for polarization 2
#define FITSIDI_INTERFEROMETER_MODEL_COLUMN_DDISP_2 {"DDISP_2", "1E", "sec m−2/sec Rate of change of dispersive delay for polarization 2" }

///Mandatory keywords for the FITS-IDI INTERFEROMETER_MODEL table
///2
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_TABREV {"TABREV", "I", "2" }
///Number of polynomial terms npoly
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NPOLY {"NPOLY", "I", "Number of polynomial terms npoly" }
///Number of polarizations
#define FITSIDI_INTERFEROMETER_MODEL_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations" }

///Columns for the FITS-IDI SYSTEM_TEMPERATURE table
///days Central time of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///days Duration of interval
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of interval" }
///Source ID number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Antenna number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Kelvin System temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_1(nband) {"TSYS_1", itostr("E", nband), "Kelvin System temperatures for polarization 1"}
///Kelvin Antenna temperatures for polarization 1
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_1(nband) {"TANT_1", itostr("E", nband), "Kelvin Antenna temperatures for polarization 1"}
///Kelvin System temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TSYS_2(nband) {"TSYS_2", itostr("E", nband), "Kelvin System temperatures for polarization 2"}
///Kelvin Antenna temperatures for polarization 2
#define FITSIDI_SYSTEM_TEMPERATURE_COLUMN_TANT_2(nband) {"TANT_2", itostr("E", nband), "Kelvin Antenna temperatures for polarization 2"}

///Mandatory keywords for the FITS-IDI SYSTEM_TEMPERATURE table
///1
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_TABREV {"TABREV", "I", "1" }
///Number of polarizations in the table
#define FITSIDI_SYSTEM_TEMPERATURE_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations in the table" }

///Types for x and y values
typedef enum
{
    ///None
    None = 0,
    ///Elevation in degrees
    Elevation_in_degrees = 1,
    ///Zenith angle in degrees
    Zenith_angle_in_degrees = 2,
    ///Hour angle in degrees
    Hour_angle_in_degrees = 3,
    ///Declination in degrees
    Declination_in_degrees = 4,
    ///Co-declination in degrees
    Codeclination_in_degrees = 5,
} celestial_xy_values;

///Spherical harmonic coefficients in GAIN_1 and GAIN 2
typedef enum
{
    ///A00
    A00 = 1,
    ///A10
    A10 = 2,
    ///A11E
    A11E = 3,
    ///A110
    A110 = 4,
    ///A20
    A20 = 5,
    ///A21E
    A21E = 6,
    ///A210
    A210 = 7,
    ///A22E
    A22E = 8,
    ///A220
    A220 = 9,
    ///A30
    A30 = 10,
} spherical_harmonic_coefficients;

///Columns for the FITS-IDI GAIN_CURVE table
///Antenna number
#define FITSIDI_GAIN_CURVE_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_GAIN_CURVE_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_GAIN_CURVE_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Gain curve types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_1(nband) {"TYPE_1", itostr("J", nband), "Gain curve types for polarization 1"}
///Number of terms or entries for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_1(nband) {"NTERM_1", itostr("J", nband), "Number of terms or entries for polarization 1"}
///x value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_1(nband) {"X_TYP_1", itostr("J", nband), "x value types for polarization 1"}
///y value types for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_1(nband) {"Y_TYP_1", itostr("J", nband), "y value types for polarization 1"}
///x values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_1(nband) {"X_VAL_1", itostr("E", nband), "x values for polarization 1"}
///y values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_1(ntab, nband) {"Y_VAL_1", itostr(itostr("E", ntab)",", nband), "y values for polarization 1"}
///Relative gain values for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_1(ntab, nband) {"GAIN_1", itostr(itostr("E", ntab)",", nband), "Relative gain values for polarization 1"}
///K/Jy Sensitivities for polarization 1
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_1(nband) {"SENS_1", itostr("E", nband), "K/Jy Sensitivities for polarization 1"}
///Gain curve types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_TYPE_2(nband) {"TYPE_2", itostr("J", nband), "Gain curve types for polarization 2"}
///Number of terms or entries for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_NTERM_2(nband) {"NTERM_2", itostr("J", nband), "Number of terms or entries for polarization 2"}
///x value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_TYP_2(nband) {"X_TYP_2", itostr("J", nband), "x value types for polarization 2"}
///y value types for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_TYP_2(nband) {"Y_TYP_2", itostr("J", nband), "y value types for polarization 2"}
///x values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_X_VAL_2(nband) {"X_VAL_2", itostr("E", nband), "x values for polarization 2"}
///y values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_Y_VAL_2(ntab, nband) {"Y_VAL_2", itostr(itostr("E", ntab)",", nband), "y values for polarization 2"}
///Relative gain values for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_GAIN_2(ntab, nband) {"GAIN_2", itostr(itostr("E", ntab)",", nband), "Relative gain values for polarization 2"}
///K/Jy Sensitivities for polarization 2
#define FITSIDI_GAIN_CURVE_COLUMN_SENS_2(nband) {"SENS_2", itostr("E", nband), "K/Jy Sensitivities for polarization 2"}

///Mandatory keywords for the FITS-IDI GAIN_CURVE table
///1
#define FITSIDI_GAIN_CURVE_KEYWORD_TABREV {"TABREV", "I", "1" }
///Number of polarizations in the table
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations in the table" }
///Number of tabulated values ntab
#define FITSIDI_GAIN_CURVE_KEYWORD_NO_TABS {"NO_TABS", "I", "Number of tabulated values ntab" }

///Columns for the FITS-IDI PHASE-CAL table
///days Central time of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///days Duration of interval
#define FITSIDI_PHASE_CAL_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of interval" }
///Source ID number
#define FITSIDI_PHASE_CAL_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Antenna number
#define FITSIDI_PHASE_CAL_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_PHASE_CAL_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_PHASE_CAL_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///seconds Cable calibration measurement
#define FITSIDI_PHASE_CAL_COLUMN_CABLE_CAL {"CABLE_CAL", "1D", "seconds Cable calibration measurement" }
///percent State counts for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_STATE_1(nband) {"STATE_1", itostr("E4,", nband), "percent State counts for polarization 1"}
///Hz Phase-cal tone frequencies for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_1(ntone, nband) {"PC_FREQ_1", itostr(itostr("D", ntone)",", nband), "Hz Phase-cal tone frequencies for polarization 1"}
///Real parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_1(ntone, nband) {"PC_REAL_1", itostr(itostr("E", ntone)",", nband), "Real parts of phase-cal measurements for polarization 1"}
///Imaginary parts of phase-cal measurements for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_1(ntone, nband) {"PC_IMAG_1", itostr(itostr("E", ntone)",", nband), "Imaginary parts of phase-cal measurements for polarization 1"}
///sec/sec Phase-cal rates for polarization 1
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_1(ntone, nband) {"PC_RATE_1", itostr(itostr("E", ntone)",", nband), "sec/sec Phase-cal rates for polarization 1"}
///percent State counts for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_STATE_2(nband) {"STATE_2", itostr("E4,", nband), "percent State counts for polarization 2"}
///Hz Phase-cal tone frequencies for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_FREQ_2(ntone, nband) {"PC_FREQ_2", itostr(itostr("D", ntone)",", nband), "Hz Phase-cal tone frequencies for polarization 2"}
///Real parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_REAL_2(ntone, nband) {"PC_REAL_2", itostr(itostr("E", ntone)",", nband), "Real parts of phase-cal measurements for polarization 2"}
///Imaginary parts of phase-cal measurements for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_IMAG_2(ntone, nband) {"PC_IMAG_2", itostr(itostr("E", ntone)",", nband), "Imaginary parts of phase-cal measurements for polarization 2"}
///sec/sec Phase-cal rates for polarization 2
#define FITSIDI_PHASE_CAL_COLUMN_PC_RATE_2(ntone, nband) {"PC_RATE_2", itostr(itostr("E", ntone)",", nband), "sec/sec Phase-cal rates for polarization 2"}

///Mandatory keywords for the FITS-IDI PHASE-CAL table
///2
#define FITSIDI_PHASE_CAL_KEYWORD_TABREV {"TABREV", "I", "2" }
///Number of polarizations in the table
#define FITSIDI_PHASE_CAL_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations in the table" }
///Number of tones ntone
#define FITSIDI_PHASE_CAL_KEYWORD_NO_TABS {"NO_TABS", "I", "Number of tones ntone" }

///Recommended SEVERITY codes
typedef enum
{
    ///No severity level assigned
    No_severity_level_assigned = -1,
    ///Data are known to be useless
    Data_are_known_to_be_useless = 0,
    ///Data are probably useless
    Data_are_probably_useless = 1,
    ///Data may be useless
    Data_may_be_useless = 2,
} severity_codes;

///Columns for the FITS-IDI FLAG table
///Source ID number
#define FITSIDI_FLAG_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Array number
#define FITSIDI_FLAG_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Antenna numbers
#define FITSIDI_FLAG_COLUMN_ANTS {"ANTS", "2J", "Antenna numbers" }
///Frequency setup number
#define FITSIDI_FLAG_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///days Time range
#define FITSIDI_FLAG_COLUMN_TIMERANG {"TIMERANG", "2E", "days Time range" }
///Band flags
#define FITSIDI_FLAG_COLUMN_BANDS(nband) {"BANDS", itostr("J", nband), "Band flags"}
///Channel range
#define FITSIDI_FLAG_COLUMN_CHANS {"CHANS", "2J", "Channel range" }
///Polarization flags
#define FITSIDI_FLAG_COLUMN_PFLAGS {"PFLAGS", "4J", "Polarization flags" }
///Reason for flag
#define FITSIDI_FLAG_COLUMN_REASON(n) { "REASON", itostr("", n)"A", "Reason for flag" }
///Severity code
#define FITSIDI_FLAG_COLUMN_SEVERITY {"SEVERITY", "1J", "Severity code" }

///Mandatory keywords for the FITS-IDI FLAG table
///2
#define FITSIDI_FLAG_KEYWORD_TABREV {"TABREV", "I", "2" }

///Columns for the FITS-IDI WEATHER table
///days Central time of interval
#define FITSIDI_WEATHER_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///days Duration of interval
#define FITSIDI_WEATHER_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of interval" }
///Antenna number
#define FITSIDI_WEATHER_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Centigrade Surface air temperature
#define FITSIDI_WEATHER_COLUMN_TEMPERATURE {"TEMPERATURE", "1E", "Centigrade Surface air temperature" }
///millibar Surface air pressure
#define FITSIDI_WEATHER_COLUMN_PRESSURE {"PRESSURE", "1E", "millibar Surface air pressure" }
///Centigrade Dewpoint temperature
#define FITSIDI_WEATHER_COLUMN_DEWPOINT {"DEWPOINT", "1E", "Centigrade Dewpoint temperature" }
///m s−1 Wind velocity
#define FITSIDI_WEATHER_COLUMN_WIND_VELOCITY {"WIND_VELOCITY", "1E", "m s−1 Wind velocity" }
///degrees Wind direction East from North
#define FITSIDI_WEATHER_COLUMN_WIND_DIRECTION {"WIND_DIRECTION", "1E", "degrees Wind direction East from North" }
///m−2 Water column
#define FITSIDI_WEATHER_COLUMN_WVR_H2O {"WVR_H2O", "1E", "m−2 Water column" }
///m−2 Electron column
#define FITSIDI_WEATHER_COLUMN_IONOS_ELECTRON {"IONOS_ELECTRON", "1E", "m−2 Electron column" }

///Mandatory keywords for the FITS-IDI WEATHER table
///2
#define FITSIDI_WEATHER_KEYWORD_TABREV {"TABREV", "I", "2" }
///Reference date
#define FITSIDI_WEATHER_KEYWORD_RDATE {"RDATE", "D", "Reference date" }

///Columns for the FITS-IDI BASELINE table
///days Central time of interval
#define FITSIDI_BASELINE_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///Source ID number
#define FITSIDI_BASELINE_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Array number
#define FITSIDI_BASELINE_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Antenna numbers forming baseline
#define FITSIDI_BASELINE_COLUMN_ANTENNA_NOS {"ANTENNA_NOS.", "2J", "Antenna numbers forming baseline" }
///Frequency setup number
#define FITSIDI_BASELINE_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Real part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_REAL_M(nstokes, nband) {"REAL_M", itostr(itostr("E", nstokes)",", nband), "Real part of multiplicative correction"}
///Imaginary part of multiplicative correction
#define FITSIDI_BASELINE_COLUMN_IMAG_M(nstokes, nband) {"IMAG_M", itostr(itostr("E", nstokes)",", nband), "Imaginary part of multiplicative correction"}
///Real part of additive correction
#define FITSIDI_BASELINE_COLUMN_REAL_A(nstokes, nband) {"REAL_A", itostr(itostr("E", nstokes)",", nband), "Real part of additive correction"}
///Imaginary part of additive correction
#define FITSIDI_BASELINE_COLUMN_IMAG_A(nstokes, nband) {"IMAG_A", itostr(itostr("E", nstokes)",", nband), "Imaginary part of additive correction"}

///Mandatory keywords for the FITS-IDI BASELINE table
///1
#define FITSIDI_BASELINE_KEYWORD_TABREV {"TABREV", "I", "1" }
///Maximum antenna number in the table
#define FITSIDI_BASELINE_KEYWORD_NO_ANT {"NO_ANT", "I", "Maximum antenna number in the table" }

///Columns for the FITS-IDI BANDPASS table
///days Central time of interval
#define FITSIDI_BANDPASS_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///days Duration of interval
#define FITSIDI_BANDPASS_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of interval" }
///Source ID number
#define FITSIDI_BANDPASS_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Antenna number
#define FITSIDI_BANDPASS_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_BANDPASS_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_BANDPASS_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Hz Channel bandwidth
#define FITSIDI_BANDPASS_COLUMN_BANDWIDTH {"BANDWIDTH", "1E", "Hz Channel bandwidth" }
///Hz Frequency of each band
#define FITSIDI_BANDPASS_COLUMN_BAND_FREQ(nband) {"BAND_FREQ", itostr("D", nband), "Hz Frequency of each band"}
///Reference antenna for polarization 1
#define FITSIDI_BANDPASS_COLUMN_REFANT_1 {"REFANT_1", "1J", "Reference antenna for polarization 1" }
///Real part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BREAL_1(nbach, nband) {"BREAL_1", itostr(itostr("E", nbach)",", nband), "Real part of bandpass correction for polarization 1"}
///Imaginary part of bandpass correction for polarization 1
#define FITSIDI_BANDPASS_COLUMN_BIMAG_1(nbach, nband) {"BIMAG_1", itostr(itostr("E", nbach)",", nband), "Imaginary part of bandpass correction for polarization 1"}
///Reference antenna for polarization 2
#define FITSIDI_BANDPASS_COLUMN_REFANT_2 {"REFANT_2", "1J", "Reference antenna for polarization 2" }
///Real part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BREAL_2(nbach, nband) {"BREAL_2", itostr(itostr("E", nbach)",", nband), "Real part of bandpass correction for polarization 2"}
///Imaginary part of bandpass correction for polarization 2
#define FITSIDI_BANDPASS_COLUMN_BIMAG_2(nbach, nband) {"BIMAG_2", itostr(itostr("E", nbach)",", nband), "Imaginary part of bandpass correction for polarization 2"}

///Mandatory keywords for the FITS-IDI BANDPASS table
///1
#define FITSIDI_BANDPASS_KEYWORD_TABREV {"TABREV", "I", "1" }
///Maximum antenna number in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_ANT {"NO_ANT", "I", "Maximum antenna number in the table" }
///Number of polarizations in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations in the table" }
///Number of spectral channels in the table
#define FITSIDI_BANDPASS_KEYWORD_NO_BACH {"NO_BACH", "I", "Number of spectral channels in the table" }
///Data channel number for first channel in the table
#define FITSIDI_BANDPASS_KEYWORD_STRT_CHN {"STRT_CHN", "I", "Data channel number for first channel in the table" }

///Columns for the FITS-IDI CALIBRATION table
///days Central time of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///days Duration of interval
#define FITSIDI_CALIBRATION_COLUMN_TIME_INTERVAL {"TIME_INTERVAL", "1E", "days Duration of interval" }
///Source ID number
#define FITSIDI_CALIBRATION_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Antenna number
#define FITSIDI_CALIBRATION_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_CALIBRATION_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_CALIBRATION_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///Kelvin System temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TSYS_1(nband) {"TSYS_1", itostr("E", nband), "Kelvin System temperature for polarization 1"}
///Kelvin Antenna temperature for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_TANT_1(nband) {"TANT_1", itostr("E", nband), "Kelvin Antenna temperature for polarization 1"}
///Kelvin/Jy Sensitivity at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_1(nband) {"SENSITIVITY_1", itostr("E", nband), "Kelvin/Jy Sensitivity at polarization 1"}
///radians Phase at polarization 1
#define FITSIDI_CALIBRATION_COLUMN_PHASE_1(nband) {"PHASE_1", itostr("E", nband), "radians Phase at polarization 1"}
///sec/sec Rate of change of delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_RATE_1(nband) {"RATE_1", itostr("E", nband), "sec/sec Rate of change of delay of polarization 1"}
///seconds Delay of polarization 1
#define FITSIDI_CALIBRATION_COLUMN_DELAY_1(nband) {"DELAY_1", itostr("E", nband), "seconds Delay of polarization 1"}
///Complex gain real part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REAL_1(nband) {"REAL_1", itostr("E", nband), "Complex gain real part for polarization 1"}
///Complex gain imaginary part for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_IMAG_1(nband) {"IMAG_1", itostr("E", nband), "Complex gain imaginary part for polarization 1"}
///Reliability weight of complex gain for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_1(nband) {"WEIGHT_1", itostr("E", nband), "Reliability weight of complex gain for polarization 1"}
///Reference antenna for polarization 1
#define FITSIDI_CALIBRATION_COLUMN_REFANT_1(nband) {"REFANT_1", itostr("J", nband), "Reference antenna for polarization 1"}
///Kelvin System temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TSYS_2(nband) {"TSYS_2", itostr("E", nband), "Kelvin System temperature for polarization 2"}
///Kelvin Antenna temperature for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_TANT_2(nband) {"TANT_2", itostr("E", nband), "Kelvin Antenna temperature for polarization 2"}
///Kelvin/Jy Sensitivity at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_SENSITIVITY_2(nband) {"SENSITIVITY_2", itostr("E", nband), "Kelvin/Jy Sensitivity at polarization 2"}
///radians Phase at polarization 2
#define FITSIDI_CALIBRATION_COLUMN_PHASE_2(nband) {"PHASE_2", itostr("E", nband), "radians Phase at polarization 2"}
///sec/sec Rate of change of delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_RATE_2(nband) {"RATE_2", itostr("E", nband), "sec/sec Rate of change of delay of polarization 2"}
///seconds Delay of polarization 2
#define FITSIDI_CALIBRATION_COLUMN_DELAY_2(nband) {"DELAY_2", itostr("E", nband), "seconds Delay of polarization 2"}
///Complex gain real part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REAL_2(nband) {"REAL_2", itostr("E", nband), "Complex gain real part for polarization 2"}
///Complex gain imaginary part for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_IMAG_2(nband) {"IMAG_2", itostr("E", nband), "Complex gain imaginary part for polarization 2"}
///Reliability weight of complex gain for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_WEIGHT_2(nband) {"WEIGHT_2", itostr("E", nband), "Reliability weight of complex gain for polarization 2"}
///Reference antenna for polarization 2
#define FITSIDI_CALIBRATION_COLUMN_REFANT_2(nband) {"REFANT_2", itostr("J", nband), "Reference antenna for polarization 2"}

///Mandatory keywords for the FITS-IDI CALIBRATION table
///1
#define FITSIDI_CALIBRATION_KEYWORD_TABREV {"TABREV", "I", "1" }
///Maximum antenna number in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_ANT {"NO_ANT", "I", "Maximum antenna number in the table" }
///Number of polarizations in the table
#define FITSIDI_CALIBRATION_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations in the table" }

///Columns for the FITS-IDI MODEL_COMPS table
///days Central time of interval
#define FITSIDI_MODEL_COMPS_COLUMN_TIME {"TIME", "1D", "days Central time of interval" }
///Source ID number
#define FITSIDI_MODEL_COMPS_COLUMN_SOURCE_ID {"SOURCE_ID", "1J", "Source ID number" }
///Antenna number
#define FITSIDI_MODEL_COMPS_COLUMN_ANTENNA_NO {"ANTENNA_NO", "1J", "Antenna number" }
///Array number
#define FITSIDI_MODEL_COMPS_COLUMN_ARRAY {"ARRAY", "1J", "Array number" }
///Frequency setup number
#define FITSIDI_MODEL_COMPS_COLUMN_FREQID {"FREQID", "1J", "Frequency setup number" }
///sec Atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_ATMOS {"ATMOS", "1D", "sec Atmospheric delay" }
///sec/sec Time derivative of atmospheric delay
#define FITSIDI_MODEL_COMPS_COLUMN_DATMOS {"DATMOS", "1D", "sec/sec Time derivative of atmospheric delay" }
///sec Group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GDELAY {"GDELAY", "1D", "sec Group delay" }
///sec/sec Rate of change of group delay
#define FITSIDI_MODEL_COMPS_COLUMN_GRATE {"GRATE", "1D", "sec/sec Rate of change of group delay" }
///sec “Clock” epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_1 {"CLOCK_1", "1D", "sec “Clock” epoch error" }
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_1 {"DCLOCK_1", "1D", "sec/sec Time derivative of clock error" }
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_1(nband) {"LO_OFFSET_1", itostr("E", nband), "Hz LO offset"}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_1(nband) {"DLO_OFFSET_1", itostr("E", nband), "Hz/sec Time derivative of LO offset"}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_1 {"DISP_1", "1E", "sec m−2 Dispersive delay" }
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_1 {"DDISP_1", "1E", "sec m−2/sec Time derivative of dispersive delay" }
///sec “Clock” epoch error
#define FITSIDI_MODEL_COMPS_COLUMN_CLOCK_2 {"CLOCK_2", "1D", "sec “Clock” epoch error" }
///sec/sec Time derivative of clock error
#define FITSIDI_MODEL_COMPS_COLUMN_DCLOCK_2 {"DCLOCK_2", "1D", "sec/sec Time derivative of clock error" }
///Hz LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_LO_OFFSET_2(nband) {"LO_OFFSET_2", itostr("E", nband), "Hz LO offset"}
///Hz/sec Time derivative of LO offset
#define FITSIDI_MODEL_COMPS_COLUMN_DLO_OFFSET_2(nband) {"DLO_OFFSET_2", itostr("E", nband), "Hz/sec Time derivative of LO offset"}
///sec m−2 Dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DISP_2 {"DISP_2", "1E", "sec m−2 Dispersive delay" }
///sec m−2/sec Time derivative of dispersive delay
#define FITSIDI_MODEL_COMPS_COLUMN_DDISP_2 {"DDISP_2", "1E", "sec m−2/sec Time derivative of dispersive delay" }

///Mandatory keywords for the FITS-IDI MODEL_COMPS table
///1
#define FITSIDI_MODEL_COMPS_KEYWORD_TABREV {"TABREV", "I", "1" }
///Reference date
#define FITSIDI_MODEL_COMPS_KEYWORD_RDATE {"RDATE", "D", "Reference date" }
///Number of polarizations in the table
#define FITSIDI_MODEL_COMPS_KEYWORD_NO_POL {"NO_POL", "I", "Number of polarizations in the table" }
///FFT size
#define FITSIDI_MODEL_COMPS_KEYWORD_FFT_SIZE {"FFT_SIZE", "I", "FFT size" }
///Oversampling factor
#define FITSIDI_MODEL_COMPS_KEYWORD_OVERSAMP {"OVERSAMP", "I", "Oversampling factor" }
///Zero padding factor
#define FITSIDI_MODEL_COMPS_KEYWORD_ZERO_PAD {"ZERO_PAD", "I", "Zero padding factor" }
///Tapering function ('HANNING' or 'UNIFORM')
#define FITSIDI_MODEL_COMPS_KEYWORD_TAPER_FN {"TAPER_FN", "A", "Tapering function ('HANNING' or 'UNIFORM')" }

///SDFITS Convention Table
#define FITS_TABLE_SDFITS "SINGLE DISH"

///SDFITS columns
///common FITS usage
#define SDFITS_COLUMN_OBJECT {"OBJECT", "8A", "common FITS usage" }
///common FITS keyword
#define SDFITS_COLUMN_TELESCOP {"TELESCOP", "8A", "common FITS keyword" }
///resolution may differ from spacing
#define SDFITS_COLUMN_FREQRES {"FREQRES", "D", "resolution may differ from spacing" }
///of backend, not one channel
#define SDFITS_COLUMN_BANDWID {"BANDWID", "D", "of backend, not one channel" }
///common FITS usage; JD preferable?
#define SDFITS_COLUMN_DATE_OBS {"DATE-OBS", "8A", "common FITS usage; JD preferable?" }
///UT time of day; UT seconds since Oh UT
#define SDFITS_COLUMN_TIME {"TIME", "D", "UT time of day; UT seconds since Oh UT" }
///effective integration time
#define SDFITS_COLUMN_EXPOSURE {"EXPOSURE", "D", "effective integration time" }
///system, not receiver, temperature
#define SDFITS_COLUMN_TSYS {"TSYS", "D", "system, not receiver, temperature" }
#define SDFITS_COLUMN_DATA "DATA"
#define SDFITS_COLUMN_TDIM(n) (itostr("TDIM", n))
#define SDFITS_COLUMN_TUNIT(n) (itostr("TUNIT", n))
#define SDFITS_COLUMN_CTYPE(n) (itostr("CTYPE", n))
#define SDFITS_COLUMN_CDELT(n) (itostr("CDELT", n))
#define SDFITS_COLUMN_CRPIX(n) (itostr("CRPIX", n))
#define SDFITS_COLUMN_CRVAL(n) (itostr("CRVAL", n))
#define SDFITS_COLUMN_CRPIX(n) (itostr("CRPIX", n))

///SDFITS columns
///Observer name;
#define SDFITS_COLUMN_OBSERVER {"OBSERVER", "8A", "Observer name;" }
///Observer & operator initials;
#define SDFITS_COLUMN_OBSID {"OBSID", "8A", "Observer & operator initials;" }
///Project ID;
#define SDFITS_COLUMN_PROJID {"PROJID", "8A", "Project ID;" }
///Scan number
#define SDFITS_COLUMN_SCAN {"SCAN", "D", "Scan number" }
///Type of data, observing mode;
#define SDFITS_COLUMN_OBSMODE {"OBSMODE", "8A", "Type of data, observing mode;" }
///Helpful description;
#define SDFITS_COLUMN_MOLECULE {"MOLECULE", "8A", "Helpful description;" }
///As appropriate;
#define SDFITS_COLUMN_TRANSITI {"TRANSITI", "8A", "As appropriate;" }
///Normalization of TA;
#define SDFITS_COLUMN_TEMPSCAL {"TEMPSCAL", "8A", "Normalization of TA;" }
///
#define SDFITS_COLUMN_FRONTEND {"FRONTEND", "8A", "" }
///Calibration Temp (K)
#define SDFITS_COLUMN_TCAL {"TCAL", "D", "Calibration Temp (K)" }
///Hot load temperature (K)
#define SDFITS_COLUMN_THOT {"THOT", "D", "Hot load temperature (K)" }
///Cold load temperature (K)
#define SDFITS_COLUMN_TCOLD {"TCOLD", "D", "Cold load temperature (K)" }
///Receiver Temp (K), Float
#define SDFITS_COLUMN_TRX {"TRX", "D", "Receiver Temp (K), Float" }
///Velocity definition & frame;
#define SDFITS_COLUMN_VELDEF {"VELDEF", "8A", "Velocity definition & frame;" }
///radial velocity correction; Vref - Vtel
#define SDFITS_COLUMN_VCORR {"VCORR", "D", "radial velocity correction; Vref - Vtel" }
///Observed Frequency (Hz)
#define SDFITS_COLUMN_OBSFREQ {"OBSFREQ", "D", "Observed Frequency (Hz)" }
///Image sideband freq (Hz)
#define SDFITS_COLUMN_IMAGFREQ {"IMAGFREQ", "D", "Image sideband freq (Hz)" }
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST {"LST", "D", "LST (seconds) at start of scan" }
///LST (seconds) at start of scan
#define SDFITS_COLUMN_LST {"LST", "D", "LST (seconds) at start of scan" }
///Commanded Azimuth (Degrees)
#define SDFITS_COLUMN_AZIMUTH {"AZIMUTH", "D", "Commanded Azimuth (Degrees)" }
///Commanded Elevation (Degrees)
#define SDFITS_COLUMN_ELEVATIO {"ELEVATIO", "D", "Commanded Elevation (Degrees)" }
///Opacity at signal freq
#define SDFITS_COLUMN_TAU {"TAU", "D", "Opacity at signal freq" }
///Opacity at image freq
#define SDFITS_COLUMN_TAUIMAGE {"TAUIMAGE", "D", "Opacity at image freq" }
///Opacity per unit air mass
#define SDFITS_COLUMN_TAUZENIT {"TAUZENIT", "D", "Opacity per unit air mass" }
///Decimal fraction 0..1
#define SDFITS_COLUMN_HUMIDITY {"HUMIDITY", "D", "Decimal fraction 0..1" }
///Ambient Temp (K)
#define SDFITS_COLUMN_TAMBIENT {"TAMBIENT", "D", "Ambient Temp (K)" }
///Barometer reading mm Hg
#define SDFITS_COLUMN_PRESSURE {"PRESSURE", "D", "Barometer reading mm Hg" }
///Dew point (K)
#define SDFITS_COLUMN_DEWPOINT {"DEWPOINT", "D", "Dew point (K)" }
///Wind speed m/s
#define SDFITS_COLUMN_WINDSPEE {"WINDSPEE", "D", "Wind speed m/s" }
///Degrees West of North
#define SDFITS_COLUMN_WINDDIRE {"WINDDIRE", "D", "Degrees West of North" }
///Main-beam efficiency
#define SDFITS_COLUMN_BEAMEFF {"BEAMEFF", "D", "Main-beam efficiency" }
///Antenna Aperature Efficiency
#define SDFITS_COLUMN_APEREFF {"APEREFF", "D", "Antenna Aperature Efficiency" }
///Rear spillover
#define SDFITS_COLUMN_ETAL {"ETAL", "D", "Rear spillover" }
///Accounts for forward loss
#define SDFITS_COLUMN_ETAFSS {"ETAFSS", "D", "Accounts for forward loss" }
///K per Jy
#define SDFITS_COLUMN_ANTGAIN {"ANTGAIN", "D", "K per Jy" }
///Large main-beam FWHM
#define SDFITS_COLUMN_BMAJ {"BMAJ", "D", "Large main-beam FWHM" }
///Small main-beam FWHM
#define SDFITS_COLUMN_BMIN {"BMIN", "D", "Small main-beam FWHM" }
///Beam position angle
#define SDFITS_COLUMN_BPA {"BPA", "D", "Beam position angle" }
///Site longitude (Degrees)
#define SDFITS_COLUMN_SITELONG {"SITELONG", "D", "Site longitude (Degrees)" }
///Site latitude (Degrees)
#define SDFITS_COLUMN_SITELAT {"SITELAT", "D", "Site latitude (Degrees)" }
///site elevation in meters
#define SDFITS_COLUMN_SITEELEV {"SITEELEV", "D", "site elevation in meters" }
///Epoch of observation (year)
#define SDFITS_COLUMN_EPOCH {"EPOCH", "D", "Epoch of observation (year)" }
///Equinox of coords (year)
#define SDFITS_COLUMN_EQUINOX {"EQUINOX", "D", "Equinox of coords (year)" }

///Global keywords for the SDFITS SINGLE DISH table
///Designation of Telescope.
#define SDFITS_KEYWORD_TELESCOP "TELESCOP"
///Name of observer.
#define SDFITS_KEYWORD_OBSERVER "OBSERVER"
///UT date of observation (dd/mm/yy) .
#define SDFITS_KEYWORD_DATE_OBS "DATE-OBS"
///Max spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMAX "DATAMAX"
///Min spectral value (K) - for whole file.
#define SDFITS_KEYWORD_DATAMIN "DATAMIN"

void vlbi_update_fits_key(fitsfile *fptr, int type, char* name, void *p, char* explanation, int *status);
long vlbi_alloc_fits_rows(fitsfile *fptr, unsigned long num_rows);
int vlbi_fill_fits_col(fitsfile *fptr, char* name, unsigned char *buf, unsigned long num_rows);
int vlbi_append_fits_col(fitsfile *fptr, char* name, char* format);
void vlbi_delete_fits_col(fitsfile *fptr, char* name);
fitsfile* vlbi_create_fits_(size_t *size, void **buf);
int vlbi_close_fits(fitsfile *fptr);
