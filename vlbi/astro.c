/*  OpenVLBI - Open Source Very Long Baseline Interferometry
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

#include <vlbi.h>

void vlbi_astro_alt_az_from_ra_dec(double J2000time, double Ra, double Dec, double Lat, double Long, double* Alt, double *Az)
{
    double lst = vlbi_time_J2000time_to_lst(J2000time, Long);
    double ha = vlbi_astro_get_local_hour_angle(lst, Ra);
    vlbi_astro_get_alt_az_coordinates(ha, Dec, Lat, Alt, Az);
}

double vlbi_astro_get_local_hour_angle(double Lst, double Ra)
{
    double Ha = (fmod(Lst, 24.0) - Ra);
    if (Ha < -12)
        Ha += 24.0;
    if (Ha >= 12.0)
        Ha -= 24.0;
    return Ha;
}

void vlbi_astro_get_alt_az_coordinates(double Ha, double Dec, double Lat, double* Alt, double *Az)
{
    double alt, az;
    Ha *= M_PI / 12.0;
    Dec *= M_PI / 180.0;
    Lat *= M_PI / 180.0;
    alt = asin(sin(Dec) * sin(Lat) + cos(Dec) * cos(Lat) * cos(Ha));
    az = acos((sin(Dec) - sin(alt)*sin(Lat)) / (cos(alt) * cos(Lat)));
    alt *= 180.0 / M_PI;
    az *= 180.0 / M_PI;
    if (sin(Ha) > 0.0)
        az = 360 - az;
    *Alt = alt;
    *Az = az;
}

double vlbi_astro_spectra_ratio(double *spectrum0, double *spectrum, int spectrum_size)
{
    double delta_spectrum = 0;
    for(int l = 0; l < spectrum_size; l++) {
        delta_spectrum += spectrum[l] / spectrum0[l];
    }
    delta_spectrum /= spectrum_size;
    return delta_spectrum;
}

double vlbi_astro_flux_ratio(double flux0, double flux, double delta_spectrum)
{
    return delta_spectrum * flux / flux0;
}

double vlbi_astro_estimate_temperature_ratio(double rad_ratio, double flux_ratio)
{
    return pow(flux_ratio / (pow(rad_ratio, 2) * BOLTSMANN), 0.25);
}

double vlbi_astro_estimate_size_ratio(double luminosity_ratio, double temperature_ratio)
{
    return sqrt(luminosity_ratio/(4*M_PI*BOLTSMANN*pow(temperature_ratio, 4.0)));
}

double vlbi_astro_estimate_luminosity_ratio(double size_ratio, double flux_ratio)
{
    return pow(size_ratio, 2) * M_PI * 4 * flux_ratio;
}

double vlbi_astro_estimate_distance_ratio(double luminosity_ratio, double flux_ratio)
{
    return sqrt(luminosity_ratio / (flux_ratio * M_PI * 4));
}

double vlbi_astro_estimate_distance_parallax(double rad, double baseline)
{
    return baseline * tan(M_PI+rad);
}

double vlbi_astro_estimate_redshift(double wavelength0, int wavelength)
{
    return (wavelength-wavelength0) / wavelength0;
}

double vlbi_astro_estimate_size_transient(double transient_object_velocity, double transit_time)
{
    return transient_object_velocity / transit_time;
}

double vlbi_astro_redshift_adjust(double distance, double redshift)
{
    return distance * pow(redshift + 1, 2);
}
