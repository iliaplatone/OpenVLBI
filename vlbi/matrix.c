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

double vlbi_matrix_estimate_resolution_zero(double frequency)
{
    return AIRY * vlbi_astro_mean_speed(0) / frequency;
}

double vlbi_matrix_estimate_resolution(double resolution_zero, double baseline)
{
    return resolution_zero / baseline;
}

double* vlbi_matrix_calc_3d_projection(double alt, double az, double *baseline)
{
    double* proj = (double*)calloc(sizeof(double), 3);
    az *= M_PI / 180.0;
    alt *= M_PI / 180.0;
    double x = baseline[0];
    double y = baseline[1];
    double z = baseline[2];
    proj[0] = (x * sin(az) + y * cos(az));
    proj[1] = (y * sin(alt) * sin(az) - x * sin(alt) * cos(az) + z * cos(alt));
    proj[2] = (cos(az) * y * cos(alt) - x * sin(az) * cos(alt) + sin(alt) * z);
    return proj;
}

double* vlbi_matrix_calc_parametric_projection(double *target, double *baseline)
{
    double* proj = (double*)calloc(sizeof(double), 3);
    target[0] *= M_PI / 180.0;
    target[1] *= M_PI / 180.0;
    double x = baseline[0];
    double y = baseline[1];
    double z = baseline[2];
    proj[0] = (x * sin(target[1]) + y * cos(target[1])) / sin(atan(target[2]));
    proj[1] = (y * sin(target[0]) * sin(target[1]) - x * sin(target[0]) * cos(target[1]) + z * cos(target[0])) / sin(atan(target[2]));
    proj[2] = (cos(target[1]) * y * cos(target[0]) - x * sin(target[1]) * cos(target[0]) + sin(target[0]) * z);
    proj[2] += proj[2] * cos(atan(target[2])) / 2;
    return proj;
}

double* vlbi_matrix_calc_uv_coordinates(double *proj, double wavelength)
{
    double* uv = (double*)calloc(sizeof(double), 3);
    uv[0] = proj[0] * AIRY / wavelength;
    uv[1] = proj[1] * AIRY / wavelength;
    uv[2] = proj[2] / vlbi_astro_mean_speed(0);
    return uv;
}

double* vlbi_matrix_calc_location(double *loc)
{
    double* location = (double*)malloc(sizeof(double)*3);
    double lat, lon, el;
    lat = loc[0];
    lon = loc[1];
    el = vlbi_matrix_estimate_geocentric_elevation(loc[0], loc[2]);
    lat *= M_PI / 180.0;
    lon *= M_PI / 180.0;
    location[0] = sqrt(pow(sin(lon), 2)+pow(1.0-cos(lon), 2)) * el * (lon < 0 ? -1 : 1);
    location[1] = sqrt(pow(sin(lat), 2)+pow(1.0-cos(lat), 2)) * el * (lat < 0 ? -1 : 1);
    location[2] = el-vlbi_matrix_estimate_geocentric_elevation(loc[0], 0);
    return location;
}

double* vlbi_matrix_calc_baseline_center(double *loc1, double *loc2)
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

double vlbi_matrix_estimate_geocentric_elevation(double Lat, double El)
{
    Lat *= M_PI / 180.0;
    Lat = fabs(cos(Lat));
    El += Lat * (EARTHRADIUSEQUATORIAL - EARTHRADIUSPOLAR) + EARTHRADIUSPOLAR;
    return El;
}
