#!/bin/bash

num_nodes=4
freq=142000000
sr=1
system=xyz
radec=18.5,38.6
lat=34
lon=15
duration=$(( $sr*0$1 ))
size=$2
type=$3

echo add context test
echo set context test
echo set location $lat,$lon,100
echo set frequency $freq
echo set samplerate $sr
echo set bitspersample 8
p=1
while (( $p<=$num_nodes )); do
        echo add node node3$p,$system,$( echo "($p*0.1*(random()%1000))" | bc ),$( echo "($p*0.1*random()%1000)" | bc ),100.0,$(head -c 10000 /dev/urandom | base64 | tr -d '\n'),$( date -u +%Y-%m-%dT%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo set resolution ${size}
echo get observation ${type}
echo quit
