#!/bin/bash

num_nodes=3
freq=1420000000
sr=1
radec=8.46,38.16
lat=44
lon=13
duration=$(( $sr*$1 ))
type=$2

echo add context test
echo set context test
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8
freq=$(echo "(2997924580/($freq*$num_nodes))" | bc -l)
p=1
tmpimg=/tmp/node
scripts/sine.sh 127 random $duration | base64 > $tmpimg
while (( $p<=$num_nodes )); do
	echo add node node3$p,$lat,$( echo "($lat+$p*3*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$lat,$( echo "($lat+$p*7*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$( echo "($lat+$p*3*$freq)" | bc -l ),$lon,100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$( echo "($lat+$p*7*$freq)" | bc -l ),$lon,100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation synthesis_${type}_geo
echo quit
