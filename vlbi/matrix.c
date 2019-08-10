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

double vlbi_estimate_elevation(double el, double latitude)
{
    return el + EARTHRADIUSEQUATORIAL - (EARTHRADIUSEQUATORIAL - EARTHRADIUSPOLAR) * latitude / 90.0;
}

double* vlbi_convert_coordinates_to_meters(double *loc)
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

double vlbi_distance_delay(double distance_m)
{
    return distance_m / SPEED_MEAN;
}

double* vlbi_baseline_from_distance(double *loc1, double *loc2)
{
    double* baseline = (double*)calloc(sizeof(double), 4);
    baseline[0] = loc1[0] - loc2[0];
    baseline[1] = loc1[1] - loc2[1];
    baseline[2] = loc1[2] - loc2[2];
    baseline[3] = sqrt(pow(baseline[0], 2) + pow(baseline[1], 2) + pow(baseline[2], 2));
    return baseline;
}

double* vlbi_baseline_from_coordinates(double *loc1, double *loc2)
{
    double* location1 = vlbi_convert_coordinates_to_meters(loc1);
    double* location2 = vlbi_convert_coordinates_to_meters(loc2);
    return vlbi_baseline_from_distance(location1, location2);
}

double* vlbi_baseline_center(double *loc1, double *loc2)
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
    return center;
}

double* vlbi_calc_baseline_projection(double DeltaAlt, double DeltaAz, double baseline_zero)
{
    double* delta = (double*)malloc(sizeof(double)*3);
    delta[0] = baseline_zero * sin(DeltaAz);
    delta[1] = baseline_zero * sin(DeltaAlt);
    delta[2] = baseline_zero * cos(DeltaAlt-M_PI/2) * sin(DeltaAz);
    return delta;
}
