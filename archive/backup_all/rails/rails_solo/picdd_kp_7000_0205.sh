#!/bin/shactor -d0 -n picdd_kp_7000_0205 -h 10.5.1.10 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.10 9700 :kp 02 7000shell msleep 3010.5.1.10 9700 :kp 05 7000shell msleep 30CONFIG