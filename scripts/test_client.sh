#!/bin/bash

num_nodes=$1
duration=$(( $2*$4/256+1 ))
freq=$3
sr=$4
radec=$5
(
echo add context test
echo set context test
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8

p=0
tmpimg=/tmp/node.tmp
while (( $duration>=0 )); do
	scripts/sine.sh 127 triwave
        duration=$(( $duration-1 ))
done | base64 > $tmpimg

while (( $p<$num_nodes )); do
	echo add node node$p,13.0027$RANDOM,44.0027$RANDOM,100.$RANDOM,$tmpimg,2000/01/01-00:00:00.000000000
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation earth_tide_dft_geo
echo quit
)
