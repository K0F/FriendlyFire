#!/bin/bashactor -d1 -n optoFE1_on -h 10.5.1.15 -p 9850 << CONFIG10.5.1.15 9800 midi   note_on   midi_channel 0    low   0x56   hi    0x56    low_velocity   0x01   hi_velocity    0x70    end10.5.1.15 9900 :FEK02CONFIG