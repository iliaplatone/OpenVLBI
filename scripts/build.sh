#!/bin/sh

mkdir -p build
pushd build
cmake ..
make
make install
popd
