#!/bin/bash

(( $#<1 )) && echo "usage: $0 num_nodes" && exit 22
set -x -e

port=8000
sudo killall indiserver -9 2>/dev/null || true
sudo killall vlbi_client_indi -9 2>/dev/null || true
vlbi_server start INDI
sleep 10
p=0
while (( $p<$1 )); do
	set -x -e
	current_port=$(( $port+$p ))
	indiserver -p $current_port indi_simulator_ccd indi_simulator_telescope indi_simulator_dome indi_simulator_gps 2>/dev/null 1>/dev/null &
	sleep 2
	vlbi_server add node "localhost:$current_port"
	p=$(( $p+1 ))
done
sleep 2
vlbi_server set connection on
sleep 2
vlbi_server add context test
sleep 2
vlbi_server set context test
sleep 2
vlbi_server set parking off
sleep 2
vlbi_server set tracking on
sleep 2
vlbi_server set target 5.34,16.34
sleep 2
vlbi_server set exposure 0.5
sleep 2
vlbi_server get observation fft
sleep 10
vlbi_server stop
sleep 10
