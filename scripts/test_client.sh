#!/bin/bash

size=$1
num_nodes=$2
duration=$3
freq=$4
bw=$5
sr=$6
radec=$7
time=$8
(
echo add context test
echo set context test
echo set frequency $freq
echo set bandwidth $bw
echo set samplerate $sr

p=0
tmpimg=/tmp/$RANDOM.tmp
sudo dd if=/dev/urandom bs=$sr count=$duration 2>/dev/null | base64 > $tmpimg
while (( $p<$num_nodes )); do
	echo add node node$p,13.0027$RANDOM,44.0027$RANDOM,100.$RANDOM,$tmpimg,$time
	p=$(( $p+1 ))
done
rm -f $tmpimg

echo set target $radec
echo set resolution $size
echo get observation earth_tide_raw_geo
echo get observation earth_tide_dft_geo
echo quit
)
