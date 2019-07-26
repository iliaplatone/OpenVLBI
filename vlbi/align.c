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

/*
dsp_star findstar(dsp_stream_p stream, dsp_stream_p star, double trigger)
{
	dsp_convolution_crosscorrelation(stream, star);
	dsp_buffer_stretch(stream, 0, 100);
	for(int x = 0; x < stream->len; x++) {
		if(stream->buffer[x] > trigger) {
			int * pos = dsp_stream_get_position(stream, x);
			while(stream->buffer[x] > trigger) {
				for(int y = 0; y < stream->len; y++) {
					
				}
			}
		}
	}
	return ret;
}*/
