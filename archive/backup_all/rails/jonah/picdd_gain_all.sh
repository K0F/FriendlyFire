#!/bin/shactor -d0 -n picdd_gain_all -h 10.5.1.3 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.3 9700 :gain 01shell msleep 3010.5.1.3 9700 :gain 02shell msleep 3010.5.1.3 9700 :gain 03shell msleep 3010.5.1.3 9700 :gain 04shell msleep 3010.5.1.3 9700 :gain 05shell msleep 3010.5.1.3 9700 :gain 06shell msleep 3010.5.1.3 9700 :gain 07shell msleep 3010.5.1.3 9700 :gain 08shell msleep 30CONFIG