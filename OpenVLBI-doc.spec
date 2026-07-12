%global forgeurl https://github.com/iliaplatone/openvlbi.git
%define __cmake_in_source_build %{_vpath_builddir}

Name: openvlbi-doc
Version: 3.0.3.git
Release: %(date -u +%%Y%%m%%d%%H%%M%%S)%{?dist}
Summary: OpenVLBI Correlator

License: LGPLv2+ and GPLv2+
# See COPYRIGHT file for a description of the licenses and files covered

Provides: libopenvlbi = %{version}-%{release}

URL: http://www.openvlbi.org
Source0: https://github.com/openvlbi/%{name}/archive/master.tar.gz

BuildRequires: cmake >= 2.4.7
BuildRequires: cmake-data
BuildRequires: libfftw3-devel
BuildRequires: libpng-devel
BuildRequires: libjpeg-devel
BuildRequires: libindi-dev|indi-full
BuildRequires: libnova-devel
BuildRequires: doxygen

BuildRequires: pkgconfig(fftw3)
BuildRequires: pkgconfig(cfitsio)
BuildRequires: pkgconfig(libcurl)
BuildRequires: pkgconfig(gsl)
BuildRequires: pkgconfig(libjpeg)
BuildRequires: pkgconfig(libusb-1.0)
BuildRequires: pkgconfig(zlib)

Requires: %{name}-libs%{?_isa} = %{version}-%{release}

%description
OpenVLBI is an open source correlator for astronomical interferometers.
with OpenVLBI astronomers can join together more telescopes to observe
celestial objects at great resolution.
OpenVLBI can be used with radio antennas or optical sensors.


%package devel
Summary: Libraries, includes, etc. used to develop an application with %{name}
Requires: %{name}-libs%{?_isa} = %{version}-%{release}
Requires: %{name}-static%{?_isa} = %{version}-%{release}

%description devel
These are the header files needed to develop a %{name} application


%package libs
Summary: OpenVLBI shared libraries

%description libs
These are the shared libraries of OpenVLBI.


%package static
Summary: Static libraries, includes, etc. used to develop an application with %{name}
Requires: %{name}-libs%{?_isa} = %{version}-%{release}

%description static
Static library needed to develop a %{name} application


%build
dnf install -y doxygen libindi-devel libnova-devel libfftw3-devel libcfitsio-devel cmake build-essential fakeroot devscripts jq
mkdir -p build
pushd build
 cmake -DCMAKE_INSTALL_PREFIX=/usr -DWITH_DOCS=On -DWITH_DATA=Off -DWITH_DSP=Off -DWITH_VLBI=Off -DWITH_VLBI_SERVER=Off ..
 make -j$(nproc)
popd

%install
sudo make install
