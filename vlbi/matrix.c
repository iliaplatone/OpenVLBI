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

double* vlbi_calc_3d_projection(double alt, double az, double *baseline)
{
    double* proj = (double*)calloc(sizeof(double), 3);
    az *= M_PI / 180.0;
    alt *= M_PI / 180.0;
    proj[0] = (baseline[0] * sin(az) + baseline[1] * cos(az));
    proj[1] = (baseline[1] * sin(alt) * sin(az) - baseline[0] * sin(alt) * cos(az) + baseline[2] * cos(alt));
    proj[2] = cos(az) * baseline[1] * cos(alt) - baseline[0] * sin(az) * cos(alt) + sin(alt) * baseline[2];
    return proj;
}

double* vlbi_calc_uv_coordinates(double *proj, double wavelength)
{
    double* uv = (double*)calloc(sizeof(double), 3);
    uv[0] = proj[0] * AIRY / wavelength;
    uv[1] = proj[1] * AIRY / wavelength;
    uv[2] = proj[2] / LIGHTSPEED;
    return uv;
}

double* vlbi_calc_location(double *loc)
{
    double lat, lon, el;
    double* location = (double*)calloc(sizeof(double), 3);
    lat = loc[0] + 90.0;
    lon = loc[1];
    el = loc[2];
    lat *= M_PI / 180.0;
    lon *= M_PI / 180.0;
    while(lat < 0)
        lat += 2 * M_PI;
    while(lon < 0)
        lon += 2 * M_PI;
    while(lat >= 2 * M_PI)
        lat -= 2 * M_PI;
    while(lon >= 2 * M_PI)
        lon -= 2 * M_PI;
    location[0] = cos(lat) * sin(lon) * el;
    location[1] = cos(lat) * cos(lon) * el;
    location[2] = sin(lat) * el;
    return location;
}

double* vlbi_calc_baseline(double *loc1, double *loc2)
{
    double* baseline = (double*)calloc(sizeof(double), 4);
    baseline[0] = loc1[0] - loc2[0];
    baseline[1] = loc1[1] - loc2[1];
    baseline[2] = loc1[2] - loc2[2];
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
