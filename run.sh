#!/usr/bin/env bash

# Run "./run.sh" for release build or "./run.sh mock" for mock build

# Set PlatformIO build env to "release" or "mock" by passing first argument. 
# By default it's "release".
env=${1:-release}   

platformio run -e $env -t upload -t monitor