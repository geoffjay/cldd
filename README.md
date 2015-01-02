# cldd [![Build Status](https://travis-ci.org/geoffjay/cldd.svg?branch=develop)][3]

Control Logging and Data Acquisition Daemon

Meant to function as a highly configurable system for interacting with physical
sensors and actuators. The CLD library (currently at gitorious.org/cld/libcld)
is responsible for a lot of the actual hardware interfacing, data logging, and
feedback control using a simple PID algorithm.

## Installation

    git clone https://github.com/geoffjay/cldd.git
    cd cldd

This uses ZeroMQ for which the vapi needs to be installed manually. The vapi
files come from the zmq-vala repository that has been added as a submodule, to
update the repository with all submodules do:

    git submodule init
    git submodule update

Manual compilation is then:

    ./autogen.sh
    make
    make install
