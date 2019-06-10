# Open source very long baseline interferometry application suite
libvlbi is an open source library for correlating single dimensional streams and obtain an image from them.
this repository contains the sources for building the library and a client application which correlates using the library streams captured from INDI servers by adding them using the vlbi_server script.

## Build libvlbi


```
sh scripts/build.sh
```

# Implementation

## Documentation

You can read the API documentation at [https://iliaplatone.github.io/OpenVLBI/api/](https://iliaplatone.github.io/OpenVLBI/api/)

## Using libvlbi

You can write an application using libvlbi by linking against libvlbi.so in your gcc command line:
```
gcc yourapp.c -lvlbi -o yourapp
```
Each instance of vlbi is initiated by vlbi_init():
```
vlbi_context context = vlbi_init();

...

dsp_stream_p stream1 = dsp_stream_new();
dsp_stream_add_dim(stream, elements_n);
dsp_convert(input_arr, stream1->in, n_elements);

stream1->location[0] = Latitude;
stream1->location[1] = Longitude;
stream1->location[2] = vlbi_calc_elevation_coarse(Elevation);

stream1->starttimeutc = vlbi_time_string_to_utc("2018-06-22T02:12.000154874");

vlbi_add_stream(context, stream1);

dsp_stream_p stream2 = dsp_stream_new();

...

vlbi_add_stream(context, stream2);

dsp_stream_p stream3 = dsp_stream_new();
...

vlbi_add_stream(context, stream3);
...

dsp_t* target = calloc(sizeof(vlbi_t), 3);
target[0] = RightAscension * 360.0 / 24.0;
target[1] = Declination;

dsp_t frequency = 60.0e+6;
dsp_t samplerate = 100.0e+6;

//obtain the UV plot of the observation
dsp_stream uvplot = vlbi_get_uv_plot_astro(context, u, v, target, frequency, samplerate); // this will return
//the aproximate Fourier transform of the observation taken (the more are the streams, and more
//accurate and precise is each stream and data, the less aproximate will be the result).
dsp_stream image_estimation = vlbi_get_fft_estimate(uvplot); // this is the aproximation
//of the image of the target, with aproximation basis the taken observations.
...

dsp_stream_free(uvplot);
vlbi_exit(context);

```

# OpenVLBI client using INDI libraries

OpenVLBI comes with an application which uses the core library and using an INDI server which contains a number of drivers with the necessary properties
It needs an indi server on which to connect. Such server must connect to each node to be added to the vlbi client application.
OpenVLBI nodes must contain informations about location, and should permit driving of the telescopes or antennas to the same celestial coordinates,
tracking must be supported on the mountings and each node should capture and run at the same bit depth, frequency and possibly bandwidth and gain.

The  needed INDI interfaces are so:
 - INDI::BaseDevice::TELESCOPE_INTERFACE
 - INDI::BaseDevice::GPS_INTERFACE
 - INDI::BaseDevice::DETECTOR_INTERFACE

and optionally, also for autoguiding:
 - INDI::BaseDevice::CCD_INTERFACE
 - INDI::BaseDevice::GUIDER_INTERFACE

The properties used by this client are:
 - "EQUATORIAL_EOD_COORDS" RA and DEC from telescope for gotos and slews
 - "TELESCOPE_TRACK_STATE" from telescope for tracking
 - "GEOGRAPHIC_COORDS" LATITUDE, LONGITUDE and ELEVATION to fill the UV plane and plot the frequency response
 - "DETECTOR_CAPTURE" to start capture
 - "DETECTOR_SETTINGS" set up frequency, bandwidth, sampling rate and depth, and gain of the detectors 

The vlbi client application opens a command shell and reads from stdin.
Format of commands is:
cmd arg value:type
where type can be string or numeric.

Here is the current command list:

```
add context name:string - add an OpenVLBI context to the internal list
set model name:string - new comparison model
set context name:string - set current OpenVLBI context selecting it by name from the internal list
set freq value:numeric - set detectors frequency
set bitspersample value:numeric - set detectors sample bit depth
set gain value:numeric - set detectors gain
set bandwidth value:numeric - set detectors bandwidth
set samplerate value:numeric - set detectors sampling rate
set target ra,dec:numeric,numeric - set telescopes celestial target
set parking state:string - set telescopes parking state (on, off)
set tracking state:string - set telescopes tracking state (on, off)
set connection state:string - set nodes connection state (on, off)
get observation type:string - get UV plot observation (raw, fft, mdl) for raw observation, frequency transformed observation and FFT of the raw observation added with the comparison model
```
# OpenVLBI server

the VLBI client can be started in daemon mode using vlbi_server, which is a script that starts a local INDI server and creates two fifo files named /tmp/INDI.fifo and /tmp/OpenVLBI.fifo.
once started, you can send the commands described above by writing those direcly on the corresponding fifo or by using the command scripts.
an example:

```
~# vlbi_server start
~# vlbi_add_node 127.0.0.1 7626
~# vlbi_add_node 127.0.0.1 7627
...
~# vlbi_server add context obs1
~# vlbi_server set context obs1
~# vlbi_server set connection on
~# vlbi_server set parking off
~# vlbi_server set tracking off
~# vlbi_server set target 5.75,25.44
~# vlbi_server set frequency 24000000
~# vlbi_server set samplerate 2000000
~# vlbi_server set bandwidth 10000
~# vlbi_server set bitspersample 16
~# vlbi_server set gain 10
~# vlbi_server set capture 10
...
~# vlbi_server get observation raw
~# vlbi_server set connection off
~# vlbi_server stop
```

# References

[[1]](https://link.springer.com/article/10.1007/s10686-018-9595-0)Optical long baseline intensity interferometry: prospects for stellar physics (Jean-Pierre Rivet, Farrokh Vakili, Olivier Lai, David Vernet, Mathilde Fouché, William Guerin, Guillaume Labeyrie, Robin Kaiser)

[[2]](https://link.springer.com/article/10.1007/s11214-018-0498-1)Very Long Baseline Interferometry: Dependencies on Frequency Stability (Axel Nothnagel, Tobias Nilsson, Harald Schuh)

[[3]](https://link.springer.com/content/pdf/10.1007/978-3-319-44431-4.pdf)Interferometry and synthesis in radio astronomy (AR Thompson, JM Moran, GW Swenson)
