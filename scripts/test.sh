#!/bin/bash

function sine() {
tao=$1
type=$2
duration=$3
max=127
i=0
k=0
printf $(while (( $k<$duration )); do
i=$(( $k%$tao ))
if [ "$type"=="triwave" ]; then
    if (( $i<$(( $tao/2 )) )); then
        printf "\\%02x" "$(( $i*$max/$(( $tao/2 )) ))"
    else
        printf "\\%02x" "$(( $max*2-$i*$max/$(( $tao/2 )) ))"
    fi
elif [ "$type"=="sinewave" ]; then
    printf "\\%02x" $(echo "x = (s($i*3.1415/$tao)*$max)+$(( $max/2-1 )); scale = 0; x / 1" | bc -l)
elif [ "$type"=="quadwave" ]; then
    printf "\\%02x" $(echo "x = (sqrt(s($i*3.1415/$tao))*$max)+$(( $max/2-1 )); scale = 0; x / 1" | bc -l)
elif [ "$type"=="random" ]; then
    head -c 1 /dev/urandom
fi
k=$(( $k+1 ))
done)
}

width=128
height=128
num_nodes=2
freq=1420000000
sr=1
system=geo
radec=18.46,38.16
lat=44
lon=13
duration=$(( $sr*0$1 ))
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
        sine $(( $duration*$p )) sinewave $duration | base64 > $tmpimg
	echo add node node3$p,$system,$lat,$( echo "($lat+$p*3*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$system,$lat,$( echo "($lat+$p*7*$freq)" | bc -l ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$system,$( echo "($lat+$p*3*$freq)" | bc -l ),$lon,100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	echo add node node3$p,$system,$( echo "($lat+$p*7*$freq)" | bc -l ),$lon,100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo set resolution ${width}x${height}
echo get observation ${type}
echo quit
