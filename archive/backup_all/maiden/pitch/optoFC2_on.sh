#!/bin/bashactor -d1 -n optoFCK04_on -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 0    low   0x38    hi    0x38    low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FCK04shell sleep 210.5.1.9 9900 :FCL04CONFIG