[![CircleCi](https://circleci.com/gh/iliaplatone/OpenVLBI/tree/master.svg?style=shield)](https://circleci.com/gh/iliaplatone/OpenVLBI/?branch=master)
[![Linux](https://github.com/iliaplatone/OpenVLBI/actions/workflows/default.yml/badge.svg)](https://github.com/iliaplatone/OpenVLBI/actions/workflows/default.yml)
[![License: GPL v2](https://img.shields.io/badge/License-GPLv2-blue.svg)](https://www.gnu.org/licenses/gpl-2.0)
[![DOI](https://img.shields.io/badge/DOI-10.5281/zenodo.340341236-blue.svg)](https://zenodo.org/badge/latestdoi/340341236)


OpenVLBI is an open source library and application suite for astronomical interferometry.

Interferometry is a discipline that studies the beam or stream interference and the possibility to obtain a coherence degree between more of them.
A 2d interference plot of a single object observed from different locations (space domain Fourier plane dependent to the location of the observers) shows the fourier transform magnitude (and the phase possibly) of the radiation detected from the object observed.

libopenvlbi offers a set of functions that aim to make interferometry simpler and faster.

There is an application suite that offers a server application that uses a simple set/get scripting language, an OpenVLBI server containing an INDI client application using the same scripting format and a json server application for who's familiar with JSON/REST APIs.

This repository contains the sources for building the library and the server applications

# Build OpenVLBI

## Ubuntu/Debian

You can build OpenVLBI by running the following command:
```
sh scripts/build.sh
```
This will install all the dependencies, including latest INDI developement libraries.
If everything will be successful three .deb packages will be built and installed

## Other OSes

You'll need CMake to build OpenVLBI, and some developement packages:
+ **libopenvlbi**:libfftw3 libcfitsio libjpeg libpng
+ **vlbi_server_indi**: libindi libnova
+ **tests and scripts**: jq

# Using OpenVLBI

You can write an application using libopenvlbi by linking against libopenvlbi.so in your gcc command line:
```
gcc yourapp.c -lopenvlbi -o yourapp
```
Each instance of vlbi is initiated by vlbi_init():
```
vlbi_context context = vlbi_init();

...

// you must be familiar with the dsp_stream_p type, see "DSP API Documentation"
dsp_stream_p stream1 = dsp_stream_new();
dsp_stream_add_dim(stream1, elements_n);
dsp_stream_alloc_buffer(stream1);

//dsp_buffer_copy converts numeric types into dsp_t (aka double)
dsp_buffer_copy(input_arr, stream1->in, stream1->len);

//Latitude and longitude in degrees, elevation in meters OSL
stream1->location.geographic.lat = Latitude;
stream1->location.geographic.lon = Longitude;
stream1->location.geographic.el = Elevation;

stream1->starttimeutc = vlbi_time_string_to_timespec("2018-06-22T02:12.000154874");

//assign to this node a friendly, unique name and add it to the context, tell to OpenVLBI to use geographic conversion
vlbi_add_node(context, stream1, "location1_capture", 1);

dsp_stream_p stream2 = dsp_stream_new();

...

vlbi_add_node(context, stream2, "location2_capture", 1);

dsp_stream_p stream3 = dsp_stream_new();
...

vlbi_add_stream(context, stream3);
...

double* target = calloc(sizeof(double), 3);
//RA must be in 24H format
target[0] = RightAscension * 360.0 / 24.0;
target[1] = Declination;

double frequency = 60.0e+6;
double samplerate = 100.0e+6;

//since we're using geographic coordinates we'd calculate the delay of each baseline
int no_delay_calculation = 0;

//telescopes do not change their location during capture, so no location companion stream
int moving_baseline = 0;

//obtain the UV plot of the observation and save it as model, 
vlbi_get_uv_plot(context, "obs1_plot_model", 1024, 1024, target, frequency, samplerate, no_delay_calculation, moving_baseline, vlbi_default_delegate);

//the default delegate just multiplies each element of the stream by indexing them according to their delay from the farest node to the target
//after this operation a new model with size 1024x1024 containing the plot of the correlation degrees of all baselines will be added into the context

//you should add a model with an estimation of the phase to get an inverse fourier transform
vlbi_add_model_from_png(context, "my_phase_estimation.png", "my_phase_estimation");

 //Create a new model containing the inverse Fourier transform of your observation as magnitude component and your estimation of the phase
vlbi_get_ifft(context, "ifft_estimation", "obs1_plot_model", "my_phase_estimation");

dsp_stream_p image_estimation = vlbi_get_model(context, "ifft_estimation");

...

dsp_stream_free_buffer(image_estimation);
dsp_stream_free(image_estimation);
vlbi_exit(context);

```

You can read the API documentation at [https://iliaplatone.github.io/OpenVLBI/](https://iliaplatone.github.io/OpenVLBI/)

# OpenVLBI servers

## OpenVLBI server sample
OpenVLBI can be tested using the built vlbi_server_dummy application, which creates a subshell or takes arguments from the standard input.
the source files of the sample application make use of the vlbi_server.h source header, which contains a base class to be inherited in case that you want to build your own server or implementation.

## OpenVLBI server using INDI libraries

OpenVLBI comes with vlbi_server_indi, a server that connects to an INDI server whose drivers will be treated by OpenVLBI as nodes.
OpenVLBI nodes must contain informations about location, aperture, focal length, samplerate and observed frequency preferably, and should permit to slew their telescopes or antennas to the same celestial coordinates.
tracking must be supported by the mounts and each node should capture and run at the same bit depth, frequency and possibly bandwidth and gain.
After each capture is done, a new node will be added to the OpenVLBI context by parsing the received FITS.

The  needed INDI interfaces are so:
 - INDI::BaseDevice::TELESCOPE_INTERFACE
 - INDI::BaseDevice::GPS_INTERFACE
 - INDI::BaseDevice::DETECTOR_INTERFACE
 - INDI::BaseDevice::RECEIVER_INTERFACE

and optionally, also for autoguiding:
 - INDI::BaseDevice::CCD_INTERFACE
 - INDI::BaseDevice::GUIDER_INTERFACE

The properties used by this server are:
 - "EQUATORIAL_EOD_COORDS" RA and DEC from telescope for gotos and slews
 - "TELESCOPE_TRACK_STATE" from telescope for tracking
 - "GEOGRAPHIC_COORDS" LATITUDE, LONGITUDE and ELEVATION to fill the UV plane and plot the frequency response
 - "DETECTOR_CAPTURE" to start capture
 - "DETECTOR_SETTINGS" set up frequency, bandwidth, sampling rate and depth, and gain of the detectors 

## OpenVLBI servers usage

The vlbi server applications open a command shell and read from stdin.
Format of commands is:
cmd arg value:type
where type can be string or numeric.

Here is the current command list:

```
add context name:string - add an OpenVLBI context to the internal list
set context name:string - set current OpenVLBI context selecting it by name from the internal list
set mask name,model,mask:string,string,sting - mask the model with mask, and save the masked model into name
set shifted name:string - shift the model by its dimensions
add node name,geo|xyz,latitude|x,longitude|y,elevation|z,datafile,observationdate:string - add a node to the internal list
add plot name,projection,synch,type:string,string,string,string - add a model with the plot of the perspective projection of all nodes during the observation in format ([synthesis|movingbase],[delay|nodelay],[raw|coverage]) synthesis for aperture synthesis observation or to plot the UV coverage. delay to automatically calculate delays between nodes, nodelay means that they are already synchronized, raw will fill the perspective path with the correlation degree of the respective baseline, coverage will create a mask to apply to a phase model or a simulated magnitude.
add idft idft,magnitude,phase:string,string,string add a model named idft from the magnitude and phase models passed
add dft idft,magnitude,phase:string,string,string add the phase and magnitude models obtained from the model passed as idft
add model name,format,data:string,string,string add a new model from the base64 encoded string containing the picture file buffer, and format as [jpeg|png|fits]
set frequency value:numeric - set detectors frequency
set bitspersample value:numeric - set detectors sample bit depth
set samplerate value:numeric - set detectors sampling rate
set target ra,dec:numeric,numeric - set telescopes celestial target
set resolution WxH:numeric,numeric - set the resolution of the output picture of the Fourier plane
set location latitude,longitude,elevation:numeric,numeric - set the reference station coordinates for xyz node locations relative to this
get models - get the models list with names and dimensions
get nodes - get the nodes list with their data
get baselines - get the baselines list with their data
get model name,format:string,string get the model with name in ([png|jpeg|fits]) format, base64 encoded
del model name:string - remove a model from the current context
del node name:string - remove a node from the current context
del context name:string - remove a context from the internal list
```

### INDI server specific commands
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
