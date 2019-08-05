#!/bin/bash

num_nodes=$1
duration=$(( $2*$4 ))
freq=$3
sr=$4
radec=$5

echo add context test
echo set context test
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8

p=0
while (( $p<$num_nodes )); do
	tmpimg=/tmp/node$p
	scripts/sine.sh 127 triwave $duration | base64 > $tmpimg
	echo add node node$p,13.0027$RANDOM,44.0027$RANDOM,100.0,$tmpimg,$( date +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation earth_tide_dft_geo
echo quit
