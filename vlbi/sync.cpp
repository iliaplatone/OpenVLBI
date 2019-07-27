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

int vlbi_find_peak(double* in, int len)
{
	double max = DBL_MIN;
	int pos = 0;
	for(int i = 0; i < len; i++)
	{
		if(in[i] > max)
		{
			max = in[i];
			pos = i;
		}
	}
	return pos;
}

int vlbi_calibrate(dsp_stream_p stream1, dsp_stream_p stream2)
{
    dsp_stream_p stream = dsp_stream_copy(stream1);
    dsp_buffer_reverse(stream->buf, stream->len);
    dsp_convolution_convolution(stream, stream2);
    dsp_fourier_dft_magnitude(stream);
    int ret = vlbi_find_peak(stream->buf, stream->len);
    dsp_stream_free(stream);
    return ret;
}
/*
double *vlbi_correct_with_tdev(double *buf, double *tdev, int len)
{
    dsp_buffer_stretch(tdev, len, -1, 2);
    return dsp_buffer_deviate(buf, len, tdev, len, -1, 2);
}*/

int vlbi_calc_delay_coarse(double* haaltaz, double* baseline_rad)
{
    double *haa = (double *)calloc(sizeof(double), 3);
    haa[0] = (haaltaz[1] - 90.0) * M_PI / 180.0;
    haa[1] = haaltaz[2] * M_PI / 180.0;
    double dt = baseline_rad[3] * cos(haa[0]) * sin(haa[1]);
    return LIGHTSPEED / dt;
}
