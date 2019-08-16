/*  OpenVLBI - Open Source Very Long Baseline Interferometry
    Copyright © 2017-2019  Ilia Platone

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <vlbi.h>
#define ratio (max - min) / (mx - mn + 1)

double vlbi_astro_get_local_hour_angle(double Lst, double Ra)
{
    double Ha = Ra - Lst;
    while (Ha < 0)
        Ha += 24.0;
    while (Ha >= 24.0)
        Ha -= 24.0;
    return Ha;
}

void vlbi_get_alt_az_from_ra_dec(double J2000time, double Ra, double Dec, double Lat, double Long, double* Alt, double *Az)
{
    double lst = vlbi_time_J2000time_to_lst(J2000time, Long);
    double ha = vlbi_astro_get_local_hour_angle(lst, Ra);
    vlbi_astro_get_alt_az_coordinates(ha, Dec, Lat, Alt, Az);
}

void vlbi_astro_get_alt_az_coordinates(double Ha, double Dec, double Lat, double* Alt, double *Az)
{
    double alt, az;
    Ha *= M_PI / 12.0;
    Dec *= M_PI / 180.0;
    Lat *= M_PI / 180.0;
    alt = asin(sin(Dec) * sin(Lat) + cos(Dec) * cos(Lat) * cos(Ha));
    az = acos((sin(Dec) - sin(alt)*sin(Lat)) / (cos(alt) * cos(Lat)));
    if (sin(Ha) >= 0.0)
        az = 2*M_PI - az;
    *Alt = alt;
    *Az = az;
}

double vlbi_astro_estimate_geocentric_elevation(double Lat, double El)
{
    Lat *= M_PI / 180.0;
    Lat = sin(Lat);
    El += Lat * (EARTHRADIUSPOLAR - EARTHRADIUSEQUATORIAL);
    return El;
}

double vlbi_astro_estimate_field_rotation_rate(double Alt, double Az, double Lat)
{
    Alt *= M_PI / 180.0;
    Az *= M_PI / 180.0;
    Lat *= M_PI / 180.0;
    double ret = cos(Lat) * cos(Az) / cos(Alt);
    ret *= 180.0 / M_PI;
    return ret;
}

double vlbi_astro_estimate_field_rotation(double HA, double rate)
{
    HA *= rate;
    while(HA >= 360.0)
        HA -= 360.0;
    while(HA < 0)
        HA += 360.0;
    return HA;
}

double vlbi_astro_parsec2m(double parsec)
{
    return parsec * PARSEC;
}

double vlbi_astro_m2au(double m)
{
    return m / ASTRONOMICALUNIT;
}

double vlbi_astro_calc_delta_magnitude(double mag0, double mag, double *spectrum, int spectrum_size, int lambda)
{
    double delta_mag = 0;
    for(int l = 0; l < spectrum_size; l++) {
        delta_mag += spectrum[l] * (mag - mag0) / spectrum[lambda];
    }
    delta_mag /= spectrum_size;
    return delta_mag;
}

double vlbi_astro_estimate_absolute_magnitude(double delta_dist, double delta_mag)
{
    return sqrt(delta_dist) * delta_mag;
}

