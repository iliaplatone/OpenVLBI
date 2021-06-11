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

double vlbi_rad2as(double rad)
{
    return rad * RAD_AS;
}

double vlbi_as2rad(double as)
{
    return as / RAD_AS;
}

double vlbi_estimate_resolution_zero(double frequency)
{
    return AIRY * SPEED_MEAN / frequency;
}

double vlbi_estimate_resolution(double resolution_zero, double baseline)
{
    return resolution_zero / baseline;
}

double vlbi_estimate_snr_zero(double gain, double resolution, double bandwidth)
{
    return abs(gain * resolution * sqrt(M_PI * bandwidth));
}

double vlbi_estimate_snr(double snr_zero, double integration)
{
    return abs(snr_zero * sqrt(integration));
}

double* vlbi_calc_haaltaz(double *location, double *target, double J2000_Offset_Time)
{
    double *haaltaz = (double *)calloc(sizeof(double), 3);
    double lst = vlbi_time_J2000time_to_lst(J2000_Offset_Time, location[1]);
    haaltaz[0] = vlbi_astro_get_local_hour_angle(lst, target[0]);
    vlbi_astro_get_alt_az_coordinates(haaltaz[0], target[1], location[0], &haaltaz[1], &haaltaz[2]);
    return haaltaz;
}

double* vlbi_calc_uv_coords(double alt, double az, double *baseline, double wavelength)
{
    double* uv = (double*)calloc(sizeof(double), 2);
    az *= M_PI / 180.0;
    alt *= M_PI / 180.0;
    uv[0] = (baseline[0] * sin(az) + baseline[1] * cos(az));
    uv[1] = (baseline[1] * sin(alt) * sin(az) - baseline[0] * sin(alt) * cos(az) + baseline[2] * cos(alt));
    uv[0] *= AIRY / wavelength;
    uv[1] *= AIRY / wavelength;
}

double vlbi_calc_baseline_delay(double alt, double az, double *baseline)
{
    alt *= M_PI / 180;
    az *= M_PI / 180;
    return cos(az) * baseline[1] * cos(alt) - baseline[0] * sin(az) * cos(alt) + sin(alt) * baseline[2];
}

double vlbi_calc_delay(double distance_m)
{
    return distance_m / SPEED_MEAN;
}

double* vlbi_calc_location_m(double *loc)
{
    double* tmp = (double*)calloc(sizeof(double), 3);
    double* location = (double*)calloc(sizeof(double), 3);
    tmp[0] = loc[0] + 90.0;
    tmp[1] = loc[1];
    tmp[2] = loc[2];
    tmp[0] *= M_PI / 180.0;
    tmp[1] *= M_PI / 180.0;
    while(tmp[0] < 0)
        tmp[0] += 2 * M_PI;
    while(tmp[1] < 0)
        tmp[1] += 2 * M_PI;
    while(tmp[0] >= 2 * M_PI)
        tmp[0] -= 2 * M_PI;
    while(tmp[1] >= 2 * M_PI)
        tmp[1] -= 2 * M_PI;
    location[0] = cos(tmp[0]) * sin(tmp[1]) * tmp[2];
    location[1] = cos(tmp[0]) * cos(tmp[1]) * tmp[2];
    location[2] = sin(tmp[0]) * tmp[2];
    return location;
}

double* vlbi_calc_baseline_m_xyz(double *loc1, double *loc2)
{
    double* baseline = (double*)calloc(sizeof(double), 4);
    baseline[0] = loc1[0] - loc2[0];
    baseline[1] = loc1[1] - loc2[1];
    baseline[2] = loc1[2] - loc2[2];
    baseline[3] = sqrt(pow(baseline[0], 2) + pow(baseline[1], 2) + pow(baseline[2], 2));
    return baseline;
}

double* vlbi_calc_baseline_m(double *loc1, double *loc2)
{
    double* baseline = (double*)calloc(sizeof(double), 4);
    double* location1 = vlbi_calc_location_m(loc1);
    double* location2 = vlbi_calc_location_m(loc2);
    baseline[0] = location1[0] - location2[0];
    baseline[1] = location1[1] - location2[1];
    baseline[2] = location1[2] - location2[2];
    return baseline;
}

double* vlbi_calc_baseline_rad(double *loc1, double *loc2)
{
    double* baseline = (double*)calloc(sizeof(double), 4);
    double* location1 = (double*)calloc(sizeof(double), 3);
    double* location2 = (double*)calloc(sizeof(double), 3);
    location1[0] = loc1[0];
    location1[1] = loc1[1];
    location1[2] = loc1[2];
    location2[0] = loc2[0];
    location2[1] = loc2[1];
    location2[2] = loc2[2];
    location1[0] *= M_PI / 180.0;
    location1[1] *= M_PI / 180.0;
    location2[0] *= M_PI / 180.0;
    location2[1] *= M_PI / 180.0;
    baseline[0] = location1[0] - location2[0];
    baseline[1] = location1[1] - location2[1];
    while(baseline[1] < -M_PI)
        baseline[1] += 2 * M_PI;
    while(baseline[1] >= M_PI)
        baseline[1] -= 2 * M_PI;
    baseline[2] = 1;
    baseline[3] = sqrt(pow(baseline[0], 2) + pow(baseline[1], 2) + pow(baseline[2], 2)) * location1[2];
    return baseline;
}

double* vlbi_calc_baseline_center(double *loc1, double *loc2)
{
    double* center = (double*)calloc(sizeof(double), 3);
    double* location1 = (double*)calloc(sizeof(double), 3);
    double* location2 = (double*)calloc(sizeof(double), 3);
    location1[0] = loc1[0];
    location1[1] = loc1[1];
    location1[2] = loc1[2];
    location2[0] = loc2[0];
    location2[1] = loc2[1];
    location2[2] = loc2[2];
    center[0] = location2[0] - location1[0];
    center[1] = location2[1] - location1[1];
    center[2] = location2[2] - location1[2];
    center[0] /= 2;
    center[1] /= 2;
    center[2] /= 2;
    center[0] += location2[0];
    center[1] += location2[1];
    center[2] += location2[2];
    while(center[1] < 0.0)
        center[1] += 360.0;
    while(center[1] >= 360.0)
        center[1] -= 360.0;
    return center;
}
