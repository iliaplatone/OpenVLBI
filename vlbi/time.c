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

timespec_t vlbi_time_mktimespec(int year, int month, int dom, int hour, int minute, int second, long nanosecond)
{
    timespec_t ret;
    struct tm t_tm;
    time_t t_time;
    t_tm.tm_sec = second;
    t_tm.tm_min = minute;
    t_tm.tm_hour = hour;
    t_tm.tm_mday = dom;
    t_tm.tm_mon = month - 1;
    t_tm.tm_year = year - 1900;
    t_tm.tm_gmtoff = 0;
    t_tm.tm_zone = "UTC";
    t_time = mktime(&t_tm);
    ret.tv_sec = t_time + nanosecond / 1000000000;
    ret.tv_nsec = nanosecond % 1000000000;
    return ret;
}

double vlbi_time_timespec_to_J2000time(timespec_t tp)
{
    char time[64];
    strcpy(time, "2000-01-01T12:00:00Z");
    timespec_t j2000 = vlbi_time_string_to_timespec(time);
    return ((double)(tp.tv_sec - j2000.tv_sec) + (double)tp.tv_nsec / 1000000000.0);
}

double vlbi_time_J2000time_to_lst(double secs_since_J2000, double Long)
{
    Long *= 24.0 / 360.0;
    return fmod(24.0 * secs_since_J2000 / SIDEREAL_DAY + Long + GAMMAJ2000, 24.0);
}

timespec_t vlbi_time_J2000time_to_timespec(double secs)
{
    timespec_t ret;
    char time[64];
    strcpy(time, "2000-01-01T12:00:00Z");
    timespec_t j2000 = vlbi_time_string_to_timespec(time);
    ret.tv_sec = secs+j2000.tv_sec;
    ret.tv_nsec = ((long)(secs  * 1000000000.0)) % 1000000000;
    return ret;
}

timespec_t vlbi_time_string_to_timespec(char *time)
{
    char* k = strtok(time, "-");
    int Y = (int)strtol(k, NULL, 10);
    k = strtok(NULL, "-");
    int M = (int)strtol(k, NULL, 10);
    k = strtok(NULL, "T");
    int D = (int)strtol(k, NULL, 10);

    k = strtok(time+11, ":");
    int H = (int)strtol(k, NULL, 10);
    k = strtok(NULL, ":");
    int m = (int)strtol(k, NULL, 10);
    k = strtok(NULL, ":");
    double s = (double)atof(k);
    return vlbi_time_mktimespec(Y, M, D, H, m, floor(s), (s-floor(s))*100000000.0);
}
