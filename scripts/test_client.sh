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
	scripts/sine.sh 127 random $duration | base64 > $tmpimg
	echo add node node1$p,$( echo "0.00001*(1300000-c(2.08132)*$p*0.3333)" | bc -l ),$( echo "0.00001*(4500000+s(2.08132)*$p*0.3333)" | bc -l ),100.0,$tmpimg,$( date +%Y/%m/%d-%H:%M:%S )
	echo add node node2$p,$( echo "0.00001*(1300000-c(2.08132)*$p*0.6666)" | bc -l ),$( echo "0.00001*(4500000-s(2.08132)*$p*0.6666)" | bc -l ),100.0,$tmpimg,$( date +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$( echo "0.00001*(1300000+c(0)*$p*0.9999)" | bc -l ),$( echo "0.00001*(4500000+s(0)*$p*0.9999)" | bc -l ),100.0,$tmpimg,$( date +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation earth_tide_dft_geo
echo quit
