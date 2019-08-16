#!/bin/bash

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
