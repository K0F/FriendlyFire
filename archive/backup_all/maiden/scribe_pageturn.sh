#!/bin/bashactor -d1 -n scribe_pageturn -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 0    low   0x53   hi    0x53   low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FEK40# rotateshell msleep 50010.5.1.9 9900 :FEK20#extensionshell msleep 250010.5.1.9 9900 :FEK08# vacuumshell msleep 50010.5.1.9 9900 :FEK04# probeshell sleep 110.5.1.9 9900 :FEL40shell msleep 120010.5.1.9 9900 :FEL0810.5.1.9 9900 :FEK80# swing10.5.1.9 9900 :FEL04shell sleep 210.5.1.9 9900 :FEL20shell sleep 510.5.1.9 9900 :FEL80shell sleep 2CONFIG