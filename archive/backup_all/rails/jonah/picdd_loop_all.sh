#!/bin/shactor -d0 -n picdd_loop_all -h 10.5.1.3 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 10   endshell picdd_pos+10000_129.shshell picdd_gtraj_129.shshell sleep 2shell picdd_pos+10000_130.shshell picdd_gtraj_130.shshell sleep 2shell picdd_pos+10000_131.shshell picdd_gtraj_131.shshell sleep 12shell picdd_pos-10000_129.shshell picdd_gtraj_129.shshell sleep 2shell picdd_pos-10000_130.shshell picdd_gtraj_130.shshell sleep 2shell picdd_pos-10000_131.shshell picdd_gtraj_131.shshell sleep 12CONFIG