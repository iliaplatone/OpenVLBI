#!/bin/bash

(( $# < 1 )) && exit

json=$1

response=$(vlbi_server_json -t 4 -v -f "$json")
buffer=$(echo $response | jq .model.buffer | tr -d '"')
name=$(echo $response | jq .model.name | tr -d '"')
format=$(echo $response | jq .model.format | tr -d '"')
echo $buffer
#| base64 -d > "$name.$format"
