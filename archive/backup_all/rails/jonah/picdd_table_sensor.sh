#!/bin/shactor -d1 -n picdd_table_sensor -h 10.5.1.3 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 0   endshell msleep 100set sensor2_midi %sensor2_midi_oldshell msleep 250set diff %sensor2_midi-%sensor2_midi_oldif %diff -ge 20   {      if %diff -le -20      {         10.5.1.3 9700 :vel 131 300000      }   }        if %diff -gt 10   {      if %diff -lt 20      {      10.5.1.3 9700 :vel 131 100000      }   }if %diff -gt -10   {      if %diff -lt 10      {      10.5.1.3 9700 :vel 131 50000      }   }if %diff -gt -20   {      if %diff -le -10      {      10.5.1.3 9700 :vel 13   100000      }   }if %sensor2_midi -ge 16   {      if %sensor2_midi -lt 32      {      10.5.1.3 9700 :pos 131 20000      10.5.1.3 9700 :gtraj 131      }   }         if %sensor2_midi -ge 32   {      if %sensor2_midi -lt 48      {      10.5.1.3 9700 :pos 131 6600      10.5.1.3 9700 :gtraj 131      }   }if %sensor2_midi -ge 48   {      if %sensor2_midi -lt 64      {      10.5.1.3 9700 :pos 131 -6600      10.5.1.3 9700 :gtraj 131      }   }if %sensor2_midi -ge 64   {      if %sensor2_midi -lt 80      {      10.5.1.3 9700 :pos 131 -20000      10.5.1.3 9700 :gtraj 131      }   }CONFIG