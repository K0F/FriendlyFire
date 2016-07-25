#!/bin/bash

function shieldsUp {
  (echo -n /1100000 > /dev/ttyACM0 && sleep 5 && echo -n /2200000 > /dev/ttyACM0)&
}

function shieldsDown {
  (echo -n /1010000 > /dev/ttyACM0 && sleep 4 && echo -n /2020000 > /dev/ttyACM0)&
}

function carouselOn {
  (echo -n "/0000001" > /dev/ttyACM0)&
}

function carouselOff {
  (echo -n "/0000002" > /dev/ttyACM0)&
}

function carouselAdvance {
  (echo -n "/0001000" > /dev/ttyACM0)&
}

function allOff {
 (echo -n /2222222 > /dev/ttyACM0)&
}

function videoForward {
  (echo -n $'\x1b\x5b\x43' > /home/pi/ctl)&
}

function videoBackward {
  (echo -n $'\x1b\x5b\x44' > /home/pi/ctl)&
}
