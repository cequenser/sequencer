[![Build Status](https://travis-ci.org/cequenser/sequencer.svg?branch=master)](https://travis-ci.org/cequenser/sequencer)
[![Coverage Status](https://coveralls.io/repos/github/cequenser/sequencer/badge.svg?branch=HEAD)](https://coveralls.io/github/cequenser/sequencer?branch=HEAD)
[![Documentation](https://codedocs.xyz/cequenser/sequencer.svg)](https://codedocs.xyz/cequenser/sequencer/)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

# sequencer

Clone repo: git clone "ssh://cequenser@review.gerrithub.io:29418/cequenser/sequencer" && scp -p -P 29418 cequenser@review.gerrithub.io:hooks/commit-msg "sequencer/.git/hooks/"

Add new CI images:
 * Add Dockerfile to a new empty branch
 * Configure dockerhub (https://hub.docker.com/?namespace=cequenser) to automatically generate images on pushes to the newly created branch
