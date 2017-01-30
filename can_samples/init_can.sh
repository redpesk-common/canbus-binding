#!/bin/sh

CAN_DEVICE=can0
CAN_BITRATE=500000

echo "Check if CAN module loaded:"
lsmod | grep can > /dev/null
if [ "$?" -ne "0" ]; then
	echo -e "\tCAN module is not loaded, loading it..."
	sudo modprobe can
	if [ "$?" -ne "0" ]; then
		echo -e "\tFAILED to load CAN module !"
		exit 1
	fi
	echo -e "\tCAN module loaded."
fi

echo "Setup connection:"
sudo ip link set $CAN_DEVICE type can bitrate $CAN_BITRATE
if [ "$?" -ne 0 ]; then
	echo -e "\tFAILED to configure $CAN_DEVICE interface !"
	exit 2
fi

sudo ip link set up $CAN_DEVICE
if [ "$?" -ne 0 ]; then
	echo -e "\tFAILED to enable $CAN_DEVICE interface !"
	exit 3
fi

echo "Ready to use $CAN_DEVICE !"

