#!/bin/bash

LABEL=$1
BUILD_URL=$2
PATCHSET_REVISION=$3

sed "s/<Label>/${LABEL}/g" scripts/ci/message_only_template.json | \
sed "s/<VERIFIED>/+1/g" | \
sed "s@<MESSAGE>@${BUILD_URL}: SUCCESS@g" | \
ssh -p 29418 cequenser-ci@review.gerrithub.io gerrit review ${PATCHSET_REVISION} --json
