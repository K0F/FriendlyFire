#!/bin/bashactor -d1 -n optoFE6_off -h 10.5.1.14 -p 9850 << CONFIG10.5.1.10 9800 midi   note_off   midi_channel 0    low   0x5f   hi    0x5f    low_velocity   0x01   hi_velocity    0x70    end10.5.1.10 9900 :FEL40CONFIG