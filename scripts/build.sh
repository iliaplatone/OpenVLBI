#!/bin/bash
export version=$(head -n 1 debian/changelog | tr -d [a-z:\(:\):=:\;:\ ] | tr -d '\n')

sudo add-apt-repository ppa:~mutlaqja/ppa
sudo apt-get update
sudo apt-get install indi-full libindi-dev libnova-dev libfftw3-dev libcfitsio-dev cdbs cmake build-essential fakeroot devscripts
cd ../; tar zcvf libopenvlbi_$version.orig.tar.gz OpenVLBI; cd OpenVLBI; dpkg-buildpackage;
sudo dpkg -i ../libopenvlbi_$version.deb
sudo dpkg -i ../openvlbi-bin_$version.deb
