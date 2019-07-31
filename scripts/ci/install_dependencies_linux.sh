#!/bin/bash

COMPILER=$1
SEQUENCER_REPO=${PWD}
EXTERNAL="external/repo"

### install rtmidi
cd ${SEQUENCER_REPO}/${EXTERNAL}
mkdir -p install/rtmidi
mkdir -p build/rtmidi
cd build/rtmidi
cmake ${SEQUENCER_REPO}/${EXTERNAL}/rtmidi -D__LINUX_ALSA__=ON -DCMAKE_INSTALL_PREFIX=${SEQUENCER_REPO}/${EXTERNAL}/install/rtmidi -DCMAKE_CXX_COMPILER=${COMPILER} -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON
cmake --build . --target install

cd ${SEQUENCER_REPO}

