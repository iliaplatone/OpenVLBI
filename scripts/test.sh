#!/bin/bash

(( $#<5 )) && echo "usage: $0 uv_size num_nodes duration freq bw sr" && exit 22
set -x -e

size=$1
num_nodes=$2
duration=$3
freq=$4
bw=$5
sr=$6

vlbi_server stop
vlbi_server start dummy
vlbi_server add context test
vlbi_server set context test
vlbi_server set frequency $freq
vlbi_server set bandwidth $bw
vlbi_server set samplerate $sr

p=0
while (( $p<$num_nodes )); do
	set -x -e
	tmpimg=/tmp/$RANDOM.tmp
	sudo dd if=/dev/urandom bs=$((8*$sr)) count=$duration 2>/dev/null | base64 > $tmpimg
	vlbi_server add node 0.003$RANDOM,35.003$(( $RANDOM )),3$(( $RANDOM%10 )).$RANDOM,$tmpimg,$(date +%Y/%m/%d-%H:%M:%S).$RANDOM$RANDOM
	rm $tmpimg
	p=$(( $p+1 ))
done

vlbi_server set target $(( $RANDOM%24 )).$RANDOM,$(( $RANDOM%90 )).$RANDOM
vlbi_server set resolution $size
vlbi_server get observation earth_tide_raw_geo | base64 -d -i | convert -size $size -depth 64 -define 'quantum:format=float' gray:- png:- | file -
vlbi_server stop
sleep 10
