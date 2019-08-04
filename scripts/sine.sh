#!/bin/sh

MAX_PWM=$1
type=$2
i=0

printf $(while [ $i -lt 256 ]; do
if [ "$type"=="triwave" ]; then
    if [ $i -lt 128 ]; then
        printf "\\%02x" "$(( $i*${MAX_PWM}/128 ))"
    else
        printf "\\%02x" "$(( ${MAX_PWM}*2-$i*${MAX_PWM}/128 ))"
    fi
elif [ "$type"=="sinewave" ]; then
    printf "\\%02x" $(echo "x = (s($i*3.1415/256)*${MAX_PWM})+127; scale = 0; x / 1" | bc -l)
elif [ "$type"=="quadwave" ]; then
    printf "\\%02x" $(echo "x = (sqrt(s($i*3.1415/256))*${MAX_PWM})+127; scale = 0; x / 1" | bc -l)
fi
i=$(( $i+1 ))
done)
