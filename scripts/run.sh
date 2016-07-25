#!/bin/bash

(cat /dev/ttyACM0 &> /dev/null)&
echo "hello world" > ~/log.txt
(/usr/bin/omxplayer --loop --no-osd -b -ohdmi video.mp4 < /home/pi/ctl)&
(echo -n i > /home/pi/ctl)&

bash /home/pi/serialOn.sh
source /home/pi/master.sh


# TESTING MODE ################

(while true; do 
	sleep 10
	shieldsUp
	carouselOn
	sleep 5
	for i in `seq 1 80`; do carouselAdvance; sleep 1 ; done
	carouselOff
	shieldsDown
	allOff
	sleep 60
done)&
