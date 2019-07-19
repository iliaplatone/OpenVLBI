#!/bin/bash
sudo add-apt-repository ppa:~mutlaqja -y
sudo apt-get -qq update
sudo apt-get install indi-full libnova-dev libindi-dev libpthread-stubs0-dev libcfitsio-dev zlib1g-dev cmake dpkg-dev

mkdir -p build
pushd build
cmake -DCMAKE_INSTALL_PREFIX=../debian/openvlbi/usr ..
make
sudo make install
popd

pushd debian
./build.deb.sh openvlbi 1.4.0 $(uname -m)
dpkg -i packages/openvlbi_1.4.0_$(uname -m).deb
popd
