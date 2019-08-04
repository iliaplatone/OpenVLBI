#!/bin/bash

num_nodes=$1
duration=$(( $2*$4/256+1 ))
freq=$3
sr=$4
radec=$5

echo add context test
echo set context test
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8

tmpimg=/tmp/node.tmp
while (( $duration>=0 )); do
	scripts/sine.sh 127 triwave
        duration=$(( $duration-1 ))
done | base64 > $tmpimg

p=0
while (( $p<$num_nodes )); do
	echo add node node$p,13.0027$RANDOM,44.0027$RANDOM,100.0,$tmpimg,$( date +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation earth_tide_raw_geo
echo quit
