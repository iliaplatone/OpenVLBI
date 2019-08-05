#!/bin/bash

MAX_PWM=$1
type=$2
duration=$3
i=0
k=0
tao=$(( $RANDOM%118+20 ))
printf $(while (( $k<$duration )); do
i=$(( $k%$tao ))
if [ "$type"=="triwave" ]; then
    if (( $i<$(( $tao/2 )) )); then
        printf "\\%02x" "$(( $i*${MAX_PWM}/$(( $tao/2 )) ))"
    else
        printf "\\%02x" "$(( ${MAX_PWM}*2-$i*${MAX_PWM}/$(( $tao/2 )) ))"
    fi
elif [ "$type"=="sinewave" ]; then
    printf "\\%02x" $(echo "x = (s($i*3.1415/$tao)*${MAX_PWM})+$(( $MAX_PWM/2-1 )); scale = 0; x / 1" | bc -l)
elif [ "$type"=="quadwave" ]; then
    printf "\\%02x" $(echo "x = (sqrt(s($i*3.1415/$tao))*${MAX_PWM})+$(( $MAX_PWM/2-1 )); scale = 0; x / 1" | bc -l)
fi
k=$(( $k+1 ))
done)
