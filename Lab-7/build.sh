#!/bin/bash
mkdir -p build
cd build
cmake ..
make
cp ../deploy.prototxt .
cp ../res10_300x300_ssd_iter_140000.caffemodel .
