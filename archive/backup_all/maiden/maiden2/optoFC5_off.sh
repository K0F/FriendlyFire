#!/bin/bashactor -d1 -n optoFC5_off -h 10.5.1.15 -p 9850 << CONFIG10.5.1.15 9800 midi   note_off   midi_channel 0    low   0x39    hi    0x39    low_velocity   0x01   hi_velocity    0x70    end10.5.1.15 9900 :FCL20CONFIG