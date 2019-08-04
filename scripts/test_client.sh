#!/bin/bash

size=$1
num_nodes=$2
freq=$4
sr=$5
radec=$6
bps=32
duration=$(( $sr*$3*$bps/2048 ))
time=$7

echo add context test
echo set context test
echo set frequency $freq
echo set bandwidth $bw
echo set bitspersample $bps
echo set samplerate $sr

p=0
tmpimg=/tmp/node.tmp
while [ $duration -gt 0 ]; do
	scripts/sine.sh 127 triwave
	duration=$(( $duration-1 ))
done | base64 > $tmpimg

while (( $p<$num_nodes )); do
	echo add node node$p,13.27$RANDOM,44.27$RANDOM,100.$RANDOM,$tmpimg,$time
	p=$(( $p+1 ))
done
#rm -f $tmpimg

echo set target $radec
echo set resolution $size
echo get observation earth_tide_raw_geo
echo quit
