#!/bin/bashactor -d1 -n optoFE3_on -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 0    low   0x59   hi    0x59    low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FEK08CONFIG