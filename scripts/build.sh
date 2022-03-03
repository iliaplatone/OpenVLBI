#!/bin/bash

dir=$(dirname $0)
os=$($dir/determine-os.sh)

if [ "$os" == "Debian" ] || [ "$os" == "Ubuntu" ]; then
 if [ "$os" == "Ubuntu" ]; then
  add-apt-repository -y ppa:~mutlaqja/ppa
 fi
 version=$(head -n 1 debian/changelog | tr -d [a-z:\(:\):=:\;:\ ] | tr -d '\n')
 arch=$(dpkg --print-architecture)
 apt-get update
 apt-get install -y libindi-dev libnova-dev libfftw3-dev libcfitsio-dev cdbs cmake dpkg-dev build-essential fakeroot devscripts jq
 dpkg-buildpackage  -b -rfakeroot -us -uc;
 mkdir -p packages
 mv ../libopenvlbi_${version}_${arch}.deb packages/
 mv ../openvlbi-bin_${version}_${arch}.deb packages/
 dpkg -i packages/libopenvlbi_${version}_${arch}.deb
 dpkg -i packages/openvlbi-bin_${version}_${arch}.deb
fi
