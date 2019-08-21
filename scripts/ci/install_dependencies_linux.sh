#!/bin/bash

CXX_COMPILER=$1
C_COMPILER=$2
SEQUENCER_REPO=${PWD}
EXTERNAL="external/repo"

### install rtmidi
cd ${SEQUENCER_REPO}/${EXTERNAL}
mkdir -p install/rtmidi
mkdir -p build/rtmidi
cd build/rtmidi
cmake ${SEQUENCER_REPO}/${EXTERNAL}/rtmidi -D__LINUX_ALSA__=ON -DCMAKE_INSTALL_PREFIX=${SEQUENCER_REPO}/${EXTERNAL}/install/rtmidi -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON
cmake --build . --target install

### install portaudio
cd ${SEQUENCER_REPO}/${EXTERNAL}
tar -xvzf pa_stable_v190600_20161030.tgz
mkdir -p install/portaudio
mkdir -p build/portaudio
cd build/portaudio
cmake ${SEQUENCER_REPO}/${EXTERNAL}/portaudio -DCMAKE_INSTALL_PREFIX=${SEQUENCER_REPO}/${EXTERNAL}/install/portaudio -DCMAKE_C_COMPILER=${C_COMPILER}
cmake --build . --target install

cd ${SEQUENCER_REPO}

