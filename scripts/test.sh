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
        tmpimg=/tmp/node$p
        sine 256 sinewave $duration | base64 > $tmpimg
	echo add node node3$p,$system,$( echo "($p*0.1*(random()%1000))" | bc ),$( echo "($p*0.1*random()%1000)" | bc ),100.0,$tmpimg,$( date -u +%Y/%m/%d-%H:%M:%S )
	p=$(( $p+1 ))
done

echo set target $radec
echo set resolution ${size}
echo get observation ${type}
echo quit
