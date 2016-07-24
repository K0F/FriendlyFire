#!/bin/bash
(echo -n /1010000 > /dev/ttyACM0 && sleep 4 && echo -n /2020000 > /dev/ttyACM0)&

