#!/bin/bash
(sudo touch /forcefsck)&
(cat /dev/ttyACM0 &> /dev/null)&

echo "hello world" > ~/log.txt

(/usr/bin/omxplayer --loop --no-osd -b -ohdmi video.mp4 < /home/pi/ctl)&
(echo -n i > /home/pi/ctl)&

bash /home/pi/FriendlyFire/scripts/serialOn.sh
source /home/pi/FriendlyFire/scripts/master.sh

(while true; do 
	sleep 10
	shieldsUp
	carouselOn
	sleep 10
	for i in `seq 1 80`; do carouselAdvance; sleep 1 ; done
	shieldsDown
	sleep 1	
	carouselOff
	sleep 60
done)
