#!/bin/bash

PLAYER=$(command -v canplayer 2> /dev/null || command -v linuxcan-canplayer 2> /dev/null)
FILE=$1

if [ "$PLAYER" ]
then
	$PLAYER -I "$FILE" &
else
	echo "can-utils packages not installed"
	exit 1
fi

exit 0
