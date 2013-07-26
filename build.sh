#!/bin/bash
cd libnoisered
qmake
make
cd ../julius-4.2.3
export CFLAGS="-O3 -march=native"
export CC="gcc-4.8"
./configure
make
cd julius
make
cd ../../julius_ss
make
