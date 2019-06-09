
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
