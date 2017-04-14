#!/bin/sh
# example for setting up vcan0 and modules
sudo modprobe can
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set up can0
