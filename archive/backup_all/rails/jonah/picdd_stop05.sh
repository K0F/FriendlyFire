#!/bin/shactor -d1 -n picdd_stop05 -h 10.5.1.3 -p 9850 << CONFIG10.5.1.15 9800 midi   note_on   midi_channel 0    low   0x2b   hi    0x2b   low_velocity   0x01    hi_velocity    0x7f    end10.5.1.3 9700 :stop 05CONFIG