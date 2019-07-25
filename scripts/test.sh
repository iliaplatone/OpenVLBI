#!/bin/bash

(( $#<5 )) && echo "usage: $0 num_nodes duration freq bw sr" && exit 22
set -x -e

num_nodes=$1
duration=$2
freq=$3
bw=$4
sr=$5

p=0
vlbi_server stop
sleep 2
vlbi_server start dummy
sleep 2
vlbi_server add context test
sleep 2
vlbi_server set context test
sleep 2
vlbi_server set frequency $freq
sleep 2
vlbi_server set bandwidth $bw
sleep 2
vlbi_server set samplerate $sr
sleep 2
while (( $p<$num_nodes )); do
	set -x -e
	tmpimg=/tmp/$$.tmp
	sudo dd if=/dev/urandom bs=$((8*$sr)) count=$duration 2>/dev/null | base64 > $tmpimg
	vlbi_server add node $(( $RANDOM%360 )),$(( $RANDOM%90 )),$(( $RANDOM%4000 )),$tmpimg,$(date +%Y/%m/%d-%H:%M:%S).$RANDOM$RANDOM
	rm $tmpimg
	p=$(( $p+1 ))
done

tmpimg=/tmp/$$.tmp
sleep 2
vlbi_server get observation earth_tide_dft_geo > $tmpimg
sleep 2
#vlbi_server tiff earth_tide_dft_geo 128x128 > $tmpimg
#file $tmpimg
#rm $tmpimg
vlbi_server stop
sleep 10
