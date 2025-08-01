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
 apt-get install -y doxygen libindi-dev libopencv-dev libnova-dev libfftw3-dev libcfitsio-dev cdbs cmake dpkg-dev build-essential fakeroot devscripts jq debhelper libpng-dev libjpeg-dev
 dpkg-buildpackage  -b -rfakeroot -us -uc;
 mkdir -p packages
 mv ../libopendsp1_${version}_*.deb packages/
 mv ../libopendsp-dev_${version}_*.deb packages/
 mv ../libopenvlbi1_${version}_*.deb packages/
 mv ../libopenvlbi-dev_${version}_*.deb packages/
 mv ../openvlbi-bin_${version}_*.deb packages/
 mv ../openvlbi-data_${version}_*.deb packages/
 dpkg -i \
  packages/libopendsp1_${version}_*.deb \
  packages/libopendsp-dev_${version}_*.deb \
  packages/libopenvlbi1_${version}_*.deb \
  packages/libopenvlbi-dev_${version}_*.deb \
  packages/openvlbi-data_${version}_*.deb \
  packages/openvlbi-bin_${version}_*.deb
else
 version=$(head -n 1 debian/changelog | tr -d [a-z:\(:\):=:\;:\ ] | tr -d '\n')
 dnf install -y doxygen libindi-devel libnova-devel libfftw3-devel libcfitsio-devel cdbs cmake dpkg-devel debhelper libpng-devel libjpeg-devel build-essential fakeroot devscripts jq
 rpmbuild -bi OpenVLBI.spec
 mkdir -p packages
 mv ../libopendsp1_${version}_*.rpm packages/
 mv ../libopendsp-dev_${version}_*.rpm packages/
 mv ../libopenvlbi1_${version}_*.rpm packages/
 mv ../libopenvlbi-dev_${version}_*.rpm packages/
 mv ../openvlbi-bin_${version}_*.rpm packages/
 mv ../openvlbi-data_${version}_*.rpm packages/
 dnf install -y  \
  packages/libopendsp1_${version}_*.rpm \
  packages/libopendsp-dev_${version}_*.rpm \
  packages/libopenvlbi1_${version}_*.rpm \
  packages/libopenvlbi-dev_${version}_*.rpm \
  packages/openvlbi-data_${version}_*.rpm \
  packages/openvlbi-bin_${version}_*.rpm
fi
