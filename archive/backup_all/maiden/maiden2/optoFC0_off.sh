#!/bin/bashactor -d1 -n optoFC0_off -h 10.5.1.15 -p 9850 << CONFIG10.5.1.15 9800 midi   note_off   midi_channel 0    low   0x30    hi    0x30    low_velocity   0x01   hi_velocity    0x70    end10.5.1.15 9900 :FCL01CONFIG