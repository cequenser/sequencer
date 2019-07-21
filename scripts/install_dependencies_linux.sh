#!/bin/bash

REPO=${PWD}
EXTERNAL="external/repo"
cd ${EXTERNAL}

mkdir -p install

### install rtmidi
mkdir -p install/rtmidi
cd rtmidi && mkdir -p build && cd build
cmake .. -GNinja -D__LINUX_ALSA__=ON -DCMAKE_INSTALL_PREFIX=${REPO}/${EXTERNAL}/install/rtmidi
cmake --build . --target install
cd ../..

cd ${REPO}

