#!/bin/bash

size=$1
num_nodes=$2
freq=$4
bw=$5
sr=$6
radec=$7
duration=sr*$3/256
time=$8
(
echo add context test
echo set context test
echo set frequency $freq
echo set bandwidth $bw
echo set bps 8
echo set samplerate $sr

p=0
tmpimg=/tmp/$RANDOM.tmp
echo '' > $tmpimg
while [ $duration -gt 0 ]; do
	scrips/sine.sh 16 triwave | base64 >> $tmpimg
	duration=$(( $duration-1 ))
done
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
