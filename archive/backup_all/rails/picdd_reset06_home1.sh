#!/bin/shactor -d0 -n picdd_reset06_home1 -h 10.5.1.10 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   endshell /actors/picdd_reset06_stat.sh &shell msleep 15010.5.1.10 9700 :stat 06shell msleep 10010.5.1.10 9700 :vel 06 20000010.5.1.10 9700 :vmode 06 revshell /actors/picdd_reset06_center.sh &shell /actors/picdd_reset06_ping.sh &shell msleep 50010.5.1.10 9700 :home1 0610.5.1.10 9700 :vel 06 20000010.5.1.10 9700 :vmode 06 forCONFIG