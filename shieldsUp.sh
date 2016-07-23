#!/bin/bash
(echo -n /1100000 > /dev/ttyACM0 && sleep 5 && echo -n /2200000 > /dev/ttyACM0)&

