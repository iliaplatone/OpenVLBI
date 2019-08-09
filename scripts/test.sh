#!/bin/bash

num_nodes=$1
duration=$(( $2*$4 ))
freq=$3
sr=$4
radec=$5
lat=44
lon=0

echo add context test
echo set context test
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8
freq=$(echo "(0.0008995*299792458/($freq*$num_nodes))" | bc -l)
p=1
while (( $p<=$num_nodes )); do
	tmpimg=/tmp/node$p
	scripts/sine.sh 127 random $duration | base64 > $tmpimg
	echo add node node1$p,$( echo "($lat-c(2.08132)*$p*0.3333*$freq)" | bc -l ),$( echo "($lon+s(2.08132)*$p*0.3333*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node2$p,$( echo "($lat-c(2.08132)*$p*0.6666*$freq)" | bc -l ),$( echo "($lon-s(2.08132)*$p*0.6666*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$( echo "($lat+c(0)*$p*0.9999*$freq)" | bc -l ),$( echo "($lon+s(0)*$p*0.9999*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation synthesis_coverage_geo
echo quit
