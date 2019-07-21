#!/bin/bash

(( $#<1 )) && echo "usage: $0 num_nodes" && exit 22
set -x -e
tmpimg=/tmp/$$

port=8000
vlbi_server start
sleep 10
p=0
while (( $p<$1 )); do
	set -x -e
	current_port=$(( $port+$p ))
	indiserver -p $current_port indi_simulator_detector indi_simulator_telescope indi_simulator_dome indi_simulator_gps 2>/dev/null 1>/dev/null &
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
vlbi_server jpeg fft 128x128 > $tmpimg
file $tmpimg
vlbi_server png fft 128x128 > $tmpimg
file $tmpimg
vlbi_server bmp fft 128x128 > $tmpimg
file $tmpimg
vlbi_server tiff fft 128x128 > $tmpimg
file $tmpimg
rm $tmpimg
sleep 10
vlbi_server stop
sudo killall indiserver -9 2>/dev/null || true
sleep 10
