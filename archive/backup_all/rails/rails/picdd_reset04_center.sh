#!/bin/shactor -d1 -n picdd_reset04_center -h 10.5.1.10 -p 9850 << CONFIG10.5.1.10 9700 midi   note_on   midi_channel 4    low   0x01   hi    0x01   low_velocity   0x00    hi_velocity    0x00    end10.5.1.10 9700 :rpos 04shell msleep 6010.5.1.10 9700 :pos 04 -29500shell msleep 3010.5.1.10 9700 :gtraj 04shell sleep 710.5.1.10 9700 :rpos 04#shell find /var/run -name "actor.picdd_reset04_*.pid" -exec actorkill.sh {} \;shell cat /var/run/actor.picdd_reset04_stat.pid | xargs killshell cat /var/run/actor.picdd_reset04_ping.pid | xargs killshell cat /var/run/actor.picdd_reset04_center.pid | xargs killCONFIG