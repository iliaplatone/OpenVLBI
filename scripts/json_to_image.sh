#!/bin/bash

(( $# < 1 )) && exit

json=$1

response=$(vlbi_server_json -t 4 -v -f $json -o $json.response)
buffer=$(cat $json.response | jq .model.buffer | tr -d '"')
name=$(cat $json.response | jq .model.name | tr -d '"')
format=$(cat $json.response | jq .model.format | tr -d '"')
echo $buffer | base64 -d > "$name.$format"
