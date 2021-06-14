#!/bin/bash

num_nodes=4
freq=1420000000
sr=1
system=geo
radec=18.46,38.16
lat=44
lon=13
duration=$(( $sr*$1 ))
type=$2

echo add context test
echo set context test
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8
freq=$(echo "(12.2*299792458/($freq*$num_nodes))" | bc -l)
p=1
while (( $p<=$num_nodes )); do
        tmpimg=/tmp/node$p
        scripts/sine.sh $(( $duration*$p )) sinewave $duration | base64 > $tmpimg
	echo add node node3$p,$system,$lat,$( echo "($lat+$p*3*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$system,$lat,$( echo "($lat+$p*7*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$system,$( echo "($lat+$p*3*$freq)" | bc -l ),$lon,100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$system,$( echo "($lat+$p*7*$freq)" | bc -l ),$lon,100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo get observation ${type}
echo quit
