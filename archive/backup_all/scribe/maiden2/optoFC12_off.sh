#!/bin/bashactor -d1 -n optoFC12_off -h 10.5.1.14 -p 9850 << CONFIG10.5.1.10 9800 midi   note_off   midi_channel 0    low   0x45   hi    0x45    low_velocity   0x01   hi_velocity    0x70    end10.5.1.10 9900 :FCL1000CONFIG