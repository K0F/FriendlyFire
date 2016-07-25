 Friendly Fire scripts
======================

These bash scripts controls the basic functionality of _Friendly Fire_ installation. To initialize serial communication within console directly it is necessary to source ```serianOn.sh``` or to put following command into current environment (or edit and copy ```.bashrc``` file to user home directory).

```
stty -F /dev/<your device name> cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts -hupcl
```

Please exachange ```/dev/<your device name>``` accordingly to your system USB device node name.

```run.sh``` is a script called after system boot (via cronjob), it starts video playback which is listening to /home/pi/ctl FIFO control messages

```master.sh``` is a set of functions for controlling the rest of relay functions, they are following:

+ ```shieldsUp``` brings all the shields and moves mirror into UP position
+ ```shieldsDown``` brings shields down
+ ```carouselOn``` tunrns on the carousel projector
+ ```carouselOff``` turns carousel off
+ ```carouselAdvance``` advances carousel forward one slide per run
+ ```allOff``` turns all relayboard outputs to LOW state

All these functions runs in the background, be careful executing many of them in short period of time.

_(.. more to come)_
