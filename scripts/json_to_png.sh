#!/bin/bash

(( $# < 2 )) && exit

json=$1
png=$2

vlbi_client_json -t 1 -f $json | jq .plot.buffer | tr -d '"' | base64 -d | convert -size $(cat $json | jq .plot.resolution.width | tr -d '"')x$(cat $json | jq .plot.resolution.height | tr -d '"') -depth 8 GRAY:- $png
