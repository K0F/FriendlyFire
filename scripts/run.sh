#!/bin/bash

cat /dev/ttyACM0 &> /dev/null
echo "hello world" > ~/log.txt
(omxplayer --loop --no-osd -b -ohdmi video.mp4 < /home/pi/ctl)&
echo -n i > /home/pi/ctl
