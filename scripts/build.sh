#!/bin/bash
export version=$(head -n 1 debian/changelog | tr -d [a-z:\(:\):=:\;:\ ] | tr -d '\n')
export arch=$(dpkg --print-architecture)

sudo add-apt-repository -y ppa:~mutlaqja/ppa
sudo apt-get update
sudo apt-get install -y indi-full libindi-dev libnova-dev libfftw3-dev libcfitsio-dev cdbs cmake dpkg-dev build-essential fakeroot devscripts
dpkg-buildpackage  -b -rfakeroot -us -uc;
sudo dpkg -i ../libopenvlbi_${version}_${arch}.deb
sudo dpkg -i ../openvlbi-bin_${version}_${arch}.deb
