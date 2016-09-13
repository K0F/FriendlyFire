#!/bin/bash
(sudo touch /forcefsck)&


export DISPLAY=127.0.0.1:0.0
export XAUTHORITY=/home/pi/.Xauthority
export DBUS_SESSION_BUS_ADDRESS=unix:abstract=/tmp/dbus-BouFPQKgqg,guid=64b483d7678f2196e780849752e67d3c

#(mkfifo /home/pi/ctl)&
(cat /dev/ttyACM0 &> /dev/null)&
(cd /home/pi && aseqdump -p 20 | ./parse.sh )&
#(sudo -u pi dbus-launch --exit-with-session > /home/pi/log.txt)&


echo "hello world" >> /home/pi/log.txt

(/usr/bin/omxplayer --loop --no-osd -b -olocal video.mp4)&
(ssh -fN -R 9999:localhost:22 famuStrojGlobalKof -p5081)&
(autossh -M 20000 -f -N famuStrojGlobalKof -R 9999:localhost:22 -C)&
#(echo -n i > /home/pi/ctl)&

bash /home/pi/serialOn.sh
source /home/pi/master.sh

cd /home/pi

(while true; do 
	sleep 10
	midiOff
	sleep 1
        ./dbuscontrol.sh pause
	./dbuscontrol.sh hidevideo
	shieldsUp
	carouselOn
	sleep 10
	for i in `seq 1 20`; do carouselAdvance; sleep 7 ; done
	shieldsDown
	sleep 1	
	carouselOff
        ./dbuscontrol.sh pause
	./dbuscontrol.sh unhidevideo
	sleep 1
	midiOn
	sleep 60
done)&
