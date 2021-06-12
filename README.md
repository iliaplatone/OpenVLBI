# Interferometric correlator clients application suite
libvlbi is an open source library and application suite for astronomical interferometry.

Interferometry is a discipline that studies the beam or stream interference and the obtainability of a coherence degree between more of them.
A 2d interference plot of a single object observation from different locations (space domain Fourier plane dependent to the location of the receivers) shows the fourier transform of the light pattern of the object observed.

libvlbi offers functions to applications that use it to make interferometry simpler and faster.

The application suite is composed by a client application which opens a shell and can serve it to more other client applications.
Client applications usually are linked to other libraries and with libvlbi, or drivers that permit the capture of the streams to be correlated by the central server.
The central server offers a shell that connects to the various clients and correlates using the library streams captured from INDI servers by adding them using the vlbi_server script.

This repository contains the sources for building the library and a client application

[![Build Status](https://travis-ci.org/iliaplatone/OpenVLBI.svg?branch=master)](https://travis-ci.org/iliaplatone/OpenVLBI)

# Build libvlbi


```
sh scripts/build.sh
```

# Documentation

You can read the API documentation at [https://iliaplatone.github.io/OpenVLBI/](https://iliaplatone.github.io/OpenVLBI/)

# Using libvlbi

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
stream1->location[2] = vlbi_calc_elevation_coarse(Elevation); //estimate the geocentric elevation, given the on sea level elevation

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
dsp_stream uvplot = vlbi_get_uv_plot_aperture_synthesis(context, u, v, target, frequency, samplerate); // this will return
//the aproximate Fourier transform of the observation taken (the more are the streams, and more
//accurate and precise is each stream and data, the less aproximate will be the result).
dsp_stream image_estimation = vlbi_get_fft_estimate(uvplot); // this is the aproximation
//of the image of the target, with aproximation basis the taken observations.
...

dsp_stream_free(uvplot);
vlbi_exit(context);

```

# OpenVLBI clients

## OpenVLBI client sample
OpenVLBI can be tested using the built vlbi_client_dummy application, which creates a subshell or takes arguments from the standard input.
the source files of the sample application make use of the vlbi_client.h source header, which contains a base class to be inherited in case you want to build your own client or implementation.
the sample application can be tested using the test.sh script into the scripts/ directory of this repository:
the command-line usage is as follows:
bash scripts/test.sh [integration time in seconds] [type of plot]
where [type of plot] is: [geo|abs]_[movingbase|synthesis]_[raw|dft]

```
bash scripts/test.sh 3600 geo_synthesis_dft
```

## OpenVLBI client using INDI libraries

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

## OpenVLBI clients usage

The vlbi client applications open a command shell and read from stdin.
Format of commands is:
cmd arg value:type
where type can be string or numeric.

Here is the current command list:

```
add context name:string - add an OpenVLBI context to the internal list
set context name:string - set current OpenVLBI context selecting it by name from the internal list
add node name,latitude,longitude,elevation,datafile,observationdate:string - add a node to the internal list
del node name:string - remove a node from the internal list
set model name:string - new comparison model
set freq value:numeric - set detectors frequency
set bitspersample value:numeric - set detectors sample bit depth
set samplerate value:numeric - set detectors sampling rate
set target ra,dec:numeric,numeric - set telescopes celestial target
get observation type:string - get UV plot observation ([geo|xyz]_[synthesis|movingbase]_[raw|dft|coverage]) for aperture synthesis observation or to plot the UV coverage
```

### INDI client specific commands
```
set gain value:numeric - set detectors gain
set bandwidth value:numeric - set detectors bandwidth
set parking state:string - set telescopes parking state (on, off)
set tracking state:string - set telescopes tracking state (on, off)
set capture value:numeric - set detectors capture time in seconds
set exposure value:numeric - set ccd cameras exposure time in seconds
set connection state:string - set nodes connection state (on, off)
```

# References

[[1]](https://link.springer.com/article/10.1007/s10686-018-9595-0)Optical long baseline intensity interferometry: prospects for stellar physics (Jean-Pierre Rivet, Farrokh Vakili, Olivier Lai, David Vernet, Mathilde Fouché, William Guerin, Guillaume Labeyrie, Robin Kaiser)

[[2]](https://link.springer.com/content/pdf/10.1007/978-3-319-44431-4.pdf)Interferometry and synthesis in radio astronomy (AR Thompson, JM Moran, GW Swenson)
