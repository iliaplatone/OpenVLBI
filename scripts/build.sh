#!/bin/bash
sudo add-apt-repository ppa:~mutlaqja -y
sudo apt-get -qq update
sudo apt-get install build-essential indi-full libnova-dev libindi-dev libpthread-stubs0-dev libcfitsio-dev zlib1g-dev cmake

mkdir -p build
pushd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
popd
