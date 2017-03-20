#!/bin/bash

cat cover.svg | sed -e 's/{title}/Low level CAN binder/' \
    -e 's/font-size:87.5px/font-size:50px/g' \
    -e 's/{subtitle}//g' \
    -e 's/{version}/Version 1.0/g' \
    -e 's/{date}/March 2017/g' \
    > /tmp/cover.svg

inkscape --without-gui --export-png=../cover.jpg \
    --export-background="#ffffff" -w 1800 -h 2360 /tmp/cover.svg

inkscape --without-gui --export-png=../cover_small.jpg \
    --export-background="#ffffff" -w 200 -h 262 /tmp/cover.svg