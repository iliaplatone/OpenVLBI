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

dsp_stream_p vlbi_astro_load_spectrum(char *filename)
{
    if(strlen(filename) >= strlen("index.txt")) {
        if(!strcmp(basename(filename), "index.txt")) {
            return NULL;
        }
    }
    FILE *f = fopen(filename, "r");
    if(!f) return NULL;
    pinfo("loading spectrum from %s\n", filename);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(fsize + 1);
    fread(buf, fsize, 1, f);
    fclose(f);

    double w = 0;
    dsp_stream_p spectrum = dsp_stream_new();
    dsp_stream_add_dim(spectrum, 1);
    dsp_stream_add_dim(spectrum, 1);
    dsp_star line;
    line.center.dims = spectrum->dims;
    line.center.location = (double*)malloc(sizeof(double)*2);
    line.center.location[1] = 0;
    char str[150];
    int c = 0;
    while(c < fsize) {
        int k = 0;
        while (buf[c] != '\n') {
            str[k++] = buf[c];
            c++;
        }
        c++;
        str[k] = 0;
        k = 0;
        char value[150];
        int i = 0;
        while (str[k] != ' ') {
            value[i++] = str[k];
            k++;
        }
        k++;
        value[i] = 0;
        line.diameter = atof(value);
        char character[150];
        i = 0;
        while (str[k] != ' ') {
            character[i++] = str[k];
            k++;
        }
        k++;
        character[i] = 0;
        i = 0;
        while (str[k] != ' ') {
            value[i++] = str[k];
            k++;
        }
        k++;
        value[i] = 0;
        line.center.location[0] = atof(value)/1000000000000.0;
        char name[150];
        i = 0;
        while (str[k] != '\0') {
            name[i++] = str[k];
            k++;
        }
        k++;
        name[i] = 0;
        if(!isnan(line.diameter) && !isnan(line.center.location[0])) {
            spectrum->name[0] = 0;
            strcpy(spectrum->name, name);
            strcat(spectrum->name, ",");
            strcat(spectrum->name, character);
            strcpy(line.name, name);
            strcat(line.name, ",");
            strcat(line.name, character);
            w = fmax(w, line.center.location[0]);
            dsp_stream_add_star(spectrum, line);
        } else {
            return NULL;
        }
    }
    spectrum->sizes[0] = w;
    spectrum->len = w;
    dsp_stream_alloc_buffer(spectrum, 1);
    free(line.center.location);
    free(buf);
    qsort(spectrum->stars, spectrum->stars_count, sizeof(dsp_star), dsp_qsort_star_diameter_desc);
    return spectrum;
}

int vlbi_astro_load_spectra_catalog(char *path, dsp_stream_p **catalog, int *catalog_size)
{
    if(path == NULL)
        path = VLBI_CATALOG_PATH;
    if((*catalog) == NULL)
        *catalog = (dsp_stream_p*)malloc(sizeof(dsp_stream_p));
    if(strlen(path) >= strlen("index.txt")) {
        if(strcmp(basename(path), "index.txt")) {
            return -ENODEV;
        }
    }
    FILE *f = fopen(path, "r");
    if(!f) return -ENOENT;
    int n = 0;
    pinfo("loading catalog from %s\n", path);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(fsize + 1);
    fread(buf, fsize, 1, f);
    fclose(f);
    int w = 0, x = 0;
    char str[150];
    char *dir = dirname(path);
    int c = 0;
    while(c < fsize) {
        int k = 0;
        while (buf[c] != '\n') {
            str[k++] = buf[c];
            c++;
        }
        c++;
        str[k] = 0;
        char element[150];
        memset(element, 0, 150);
        strcpy(element, dir);
        strcat(element, "/");
        strcat(element, str);
        strcat(element, ".txt");
        int subdir = vlbi_astro_load_spectra_catalog(element, catalog, catalog_size);
        if(0 > subdir) {
            dsp_stream_p spectrum = vlbi_astro_load_spectrum(element);
            if(spectrum != NULL) {
                double refsize = 0.0;
                for(x = 0; x < spectrum->stars_count; x++)
                    refsize = fmax(spectrum->stars[x].diameter, refsize);
                for(x = 0; x < spectrum->stars_count; x++)
                    spectrum->stars[x].diameter /= refsize;
                (*catalog)[(*catalog_size)+w] = spectrum;
                w++;
                *catalog = (dsp_stream_p*)realloc(*catalog, sizeof(dsp_stream_p)*((*catalog_size)+w+1));
            }
        }
    }
    free(buf);
    (*catalog_size) = (*catalog_size) + w;
    return (*catalog_size);
}

dsp_stream_p vlbi_astro_create_reference_catalog(dsp_stream_p *catalog, int catalog_size)
{
    dsp_stream_p stream = dsp_stream_new();
    dsp_stream_add_dim(stream, 1);
    dsp_stream_add_dim(stream, 1);
    dsp_stream_alloc_buffer(stream, stream->len);
    for(int c = 0; c < catalog_size; c++) {
        dsp_stream_p element = catalog[c];
        qsort(element->stars, element->stars_count, sizeof(dsp_star), dsp_qsort_star_diameter_desc);
        for(int s = 0; s < element->stars_count; s++)
            dsp_stream_add_star(stream, element->stars[s]);
        qsort(stream->stars, stream->stars_count, sizeof(dsp_star), dsp_qsort_star_diameter_desc);
    }
    return stream;
}

void vlbi_astro_save_spectrum(dsp_stream_p stream, char *filename)
{
    FILE *f = fopen(filename, "w");
    if(f == NULL) return;
    for(int x = 0; x < stream->stars_count; x ++) {
        char *name = stream->name;
        char *character = "";
        char *suffix = "";
        if(strlen(stream->name) > 2) {
            char* k = strtok(stream->name, ",");
            if(k)
                name = k;
            k = strtok(NULL, ",");
            if(k)
                suffix = k;
            k = strtok(NULL, ",");
            if(k)
                character = k;
        }
        fprintf(f, "%ld %s %ld %s %s\n", (long)stream->stars[x].diameter, character, (long)(stream->stars[x].center.location[0]*1000000000000.0), name, suffix);
    }
    fclose(f);
}

void vlbi_astro_scan_spectrum(dsp_stream_p stream, int sample_size)
{

    stream->stars_count = 0;
    double meandev = dsp_stats_stddev(stream->buf, stream->len);
    dsp_star star;
    star.center.dims = 2;
    star.center.location = (double*)malloc(sizeof(double)*2);
    star.center.location[1] = 0;
    for(int x = 0; x < stream->len-sample_size; x ++) {
        double dev = dsp_stats_stddev(((double*)(&stream->buf[x])), sample_size);
        if(dev > meandev) {
            star.diameter = dev-meandev;
            star.center.location[0] = x;
            dsp_stream_add_star(stream, star);
        }
    }
    free(star.center.location);
    qsort(stream->stars, stream->stars_count, sizeof(dsp_star), dsp_qsort_star_diameter_desc);
}

dsp_align_info vlbi_astro_align_spectra(dsp_stream_p spectrum, dsp_stream_p catalog, int max_lines, double decimals, double min_score)
{
    int x = 0, y = 0;
    int catalog_count = catalog->stars_count;
    int spectrum_count = spectrum->stars_count;
    catalog->stars_count = Min(catalog_count, max_lines);
    spectrum->stars_count = Min(spectrum_count, max_lines);
    dsp_align_get_offset(catalog, spectrum, decimals, min_score);
    catalog->stars_count = catalog_count;
    spectrum->stars_count = spectrum_count;
    double factor = spectrum->align_info.factor[0];
    if((spectrum->align_info.err & DSP_ALIGN_NO_MATCH) == 0) {
        for(x = 0; x < spectrum->stars_count; x++) {
            spectrum->stars[x].center.location[0] -= spectrum->align_info.offset[0];
            spectrum->stars[x].center.location[0] /= spectrum->align_info.factor[0];
            dsp_star star = spectrum->stars[x];
            for(y = 0; y < catalog->stars_count; y++) {
                dsp_star star0 = catalog->stars[y];
                if((long)(star0.center.location[0]*factor) == (long)(star.center.location[0]*factor)) {
                    strcpy(spectrum->stars[x].name, catalog->stars[y].name);
                    spectrum->stars[x].diameter = vlbi_astro_estimate_temperature(spectrum->stars[x].center.location[0], spectrum->stars[x].diameter);
                    break;
                }
            }
        }
    }
    return spectrum->align_info;
}

double vlbi_astro_diff_spectra(dsp_stream_p spectrum0, dsp_stream_p spectrum, double decades)
{
    decades = pow(10, decades);
    double wavelength;
    double temp;
    double temp0;
    double flux;
    double flux0;
    double delta_spectrum = 0;
    int nlines = 0;
    int x, y;
    for(x = 0; x < spectrum0->stars_count; x++) {
        dsp_star star0 = spectrum0->stars[x];
        for(y = 0; y < spectrum->stars_count; y++) {
            dsp_star star = spectrum->stars[y];
            if((long)(star0.center.location[0]*decades) == (long)(star.center.location[0]*decades)) {
                wavelength = star0.center.location[0];
                flux = star.diameter;
                flux0 = star0.diameter;
                temp = vlbi_astro_estimate_temperature(wavelength, flux);
                temp0 = vlbi_astro_estimate_temperature(wavelength, flux0);
                delta_spectrum += temp-temp0;
                nlines ++;
                break;
            }
        }
    }
    return delta_spectrum / nlines;
}

double vlbi_astro_flux_ratio(double flux0, double flux, double delta_spectrum)
{
    return delta_spectrum * flux / flux0;
}

double vlbi_astro_estimate_flux(double wavelength, double temperature)
{
    double v = LIGHTSPEED/wavelength;
    double kT = BOLTSMANN*temperature;
    double hv = PLANK*v;
    double flux = 2.0*kT*pow(v, 2.0)/pow(LIGHTSPEED, 2.0);
    if(temperature*1000000000.0/v < 20.0)
        flux *= (hv/kT)/(pow(EULER, hv/kT)-1.0);
    return flux;
}

double vlbi_astro_estimate_temperature(double wavelength, double flux)
{
    double v = LIGHTSPEED/wavelength;
    double hv = PLANK*v;
    double temperature = flux*pow(LIGHTSPEED, 2.0)/(2.0*BOLTSMANN*pow(v, 2.0));
    double rj = temperature * BOLTSMANN / hv;
    rj -= 1.0;
    rj *= Log(rj, EULER)*hv/BOLTSMANN;
    rj = sqrt(rj);
    if(rj*1000000000.0/v > 20.0)
        temperature /= rj;
    return temperature;
}

double vlbi_astro_estimate_brightness_temperature(double wavelength, double flux)
{
    double v = LIGHTSPEED/wavelength;
    return pow(LIGHTSPEED, 2)*flux/(2.0*BOLTSMANN*pow(v, 2));
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
