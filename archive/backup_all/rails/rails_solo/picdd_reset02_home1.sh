#!/bin/shactor -d0 -n picdd_reset02_home1 -h 10.5.1.10 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   endshell picdd_reset02_stat.sh &shell msleep 15010.5.1.10 9700 :stat 02shell msleep 10010.5.1.10 9700 :vel 02 20000010.5.1.10 9700 :vmode 02 revshell picdd_reset02_center.sh &shell picdd_reset02_ping.sh &shell msleep 50010.5.1.10 9700 :home1 0210.5.1.10 9700 :vel 02 20000010.5.1.10 9700 :vmode 02 forCONFIG