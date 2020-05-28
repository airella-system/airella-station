#!/usr/bin/env bash

sudo usermod -a -G tty $USER
sudo usermod -a -G dialout $USER

# logout / login now