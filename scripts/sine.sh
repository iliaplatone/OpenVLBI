#!/bin/sh

MAX_PWM=$1
type=$2
i=0

while [ $i -lt 256 ]; do
printf \\$(
if [ "$type"=="triwave" ]; then
    if [ $i -lt 128 ]; then
        printf "%02X" "$(( $i*${MAX_PWM}/128 ))"
    else
        printf "%02X" "$(( ${MAX_PWM}*2-$i*${MAX_PWM}/128 ))"
    fi
elif [ "$type"=="sinewave" ]; then
    printf "%02X" $(echo "x = (s($i*3.1415/256)*${MAX_PWM})+127; scale = 0; x / 1" | bc -l)
elif [ "$type"=="quadwave" ]; then
    printf "%02X" $(echo "x = (sqrt(s($i*3.1415/256))*${MAX_PWM})+127; scale = 0; x / 1" | bc -l)
fi
)x
i=$(( $i+1 ))
done
