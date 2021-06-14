#!/bin/bash

arch=`dpkg --print-architecture`
sudo add-apt-repository ppa:~mutlaqja -y
sudo apt-get -qq update
sudo apt-get install -y --force-yes indi-full libnova-dev libindi-dev libpthread-stubs0-dev libcfitsio-dev zlib1g-dev cmake dpkg-dev imagemagick libfftw3-dev

mkdir -p build-${arch}
pushd build-${arch}
cmake ..
make
sudo make install
popd

pushd debian
version=`grep VLBI_VERSION_STRING ../build-${arch}/vlbi.h | cut -d '"' -f2`_`lsb_release -s -i -r | tr -d '\n'`
sudo ./build.deb.sh libopenvlbi ${version} ${arch}
sudo ./build.deb.sh openvlbi-bin ${version} ${arch}
sudo dpkg -i packages/libopenvlbi_${version}*.deb packages/openvlbi-bin_${version}*.deb
popd
