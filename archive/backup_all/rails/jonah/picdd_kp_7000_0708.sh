#!/bin/shactor -d0 -n picdd_kp_7000_0708 -h 10.5.1.3 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.3 9700 :kp 07 7000shell msleep 3010.5.1.3 9700 :kp 08 7000shell msleep 30CONFIG