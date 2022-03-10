#!/bin/bash

(( $# < 2 )) && exit

json=$1
image=$2

response=$(vlbi_server_json -t $(nproc) -f $json)
buffer=$(echo $response | jq .model.buffer | tr -d '"')
format=$(echo $response | jq .model.format | tr -d '"')
echo $buffer | base64 -d > $image.$format
