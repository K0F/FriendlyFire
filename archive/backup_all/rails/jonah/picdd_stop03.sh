#!/bin/shactor -d1 -n picdd_stop03 -h 10.5.1.3 -p 9850 << CONFIG10.5.1.15 9800 midi   note_on   midi_channel 0    low   0x28   hi    0x28   low_velocity   0x01    hi_velocity    0x7f    end10.5.1.3 9700 :stop 03CONFIG