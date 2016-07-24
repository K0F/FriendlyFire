 Friendly Fire scripts
======================

These bash scripts controls the basic functionality of _Friendly Fire_ installation. To initialize serial communication within console directly it is necessary to source ```serianOn.sh``` or to put following command into current environment (or edit and copy ```.bashrc``` file to user home directory).

```
stty -F /dev/<your device name> cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts -hupcl
```

Please exachange ```/dev/<your device name>``` accordingly to your system USB device node name.

+ ```run.sh``` is a file called after system boot (cronjob), it should start video playback which is listening to /home/pi/ctl FIFO
+ ```shieldsUp.sh``` brings all the shields and moves mirror into UP position
+ ```shieldsDown.sh``` brings shields down
+ ```carouselOn.sh``` tunrns on the carousel projector
+ ```carouselOff.sh``` turns carousel off
+ ```carouselAdvance.sh``` advances carousel forward one slide per run
+ ```allOff.sh``` turns all relayboard outputs to LOW state

All the scripts runs in the background, so be careful executing many of them in short period of time.

_(.. more to come)_
