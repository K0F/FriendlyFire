#!/bin/shactor -d1 -n picdd_sensor2-3 -h 10.5.1.10 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 0   endshell msleep 10010.5.1.10 9700 : ping 01shell msleep 150set diff2 %rail01_midi-%sensor2_midiset current2_midi %sensor2_midi   if %current2_midi -lt 80   {   if %diff2 -ge 15      {      10.5.1.10 9700 :vel 131 200000      10.5.1.10 9700 :vel 01 500000      10.5.1.10 9700 :vmode 131 for      10.5.1.10 9700 :vmode 01 for      }   }   if %current2_midi -lt 80   {   if %diff2 -lt 15      {         if %diff2 -ge 7         {         10.5.1.10 9700 :vel 131 100000         10.5.1.10 9700 :vel 01 250000         10.5.1.10 9700 :vmode 131 for         10.5.1.10 9700 :vmode 01 for         }      }   }   if %current2_midi -lt 80   {   if %diff2 -lt 7      {         if %diff2 -ge 3         {         10.5.1.10 9700 :vel 131 50000         10.5.1.10 9700 :vel 01 125000         10.5.1.10 9700 :vmode 131 for         10.5.1.10 9700 :vmode 01 for         }      }   }   if %current2_midi -lt 80	{	if %diff2 -lt 3	   {	      if %diff2 -gt -3	      {	      10.5.1.10 9700 :vel 131 0 	      10.5.1.10 9700 :vmode 131 rev      	}   	}	}   if %current2_midi -lt 80   {   if %diff2 -le -15      {      10.5.1.10 9700 :vel 131 200000      10.5.1.10 9700 :vel 01 500000      10.5.1.10 9700 :vmode 131 rev      10.5.1.10 9700 :vmode 01 rev      }   }   if %current2_midi -lt 80   {   if %diff2 -gt -15      {         if %diff2 -le -7         {         10.5.1.10 9700 :vel 131 100000         10.5.1.10 9700 :vel 01 250000         10.5.1.10 9700 :vmode 131 rev         10.5.1.10 9700 :vmode 01 rev         }      }   }if %current2_midi -lt 80   {   if %diff2 -gt -7      {         if %diff2 -le -3         {         10.5.1.10 9700 :vel 131 50000         10.5.1.10 9700 :vel 01 125000         10.5.1.10 9700 :vmode 131 rev         10.5.1.10 9700 :vmode 01 rev         }      }    }      shell msleep 10010.5.1.10 9700 : ping 04shell msleep 100set diff3 %rail04_midi-%sensor3_midi   set current3_midi %sensor3_midiif %current3_midi -lt 80   {   if %diff3 -ge 15      {      10.5.1.10 9700 :vel 132 200000      10.5.1.10 9700 :vel 04 500000      10.5.1.10 9700 :vmode 132 for      10.5.1.10 9700 :vmode 04 for     }   }   if %current3_midi -lt 80   {   if %diff3 -lt 15      {         if %diff3 -ge 7         {         10.5.1.10 9700 :vel 132 100000         10.5.1.10 9700 :vel 04 250000         10.5.1.10 9700 :vmode 132 for         10.5.1.10 9700 :vmode 04 for         }      }   }   if %current3_midi -lt 80   {   if %diff3 -lt 7      {         if %diff3 -ge 3         {         10.5.1.10 9700 :vel 132 50000         10.5.1.10 9700 :vel 04 125000         10.5.1.10 9700 :vmode 132 for         10.5.1.10 9700 :vmode 04 for         }      }   }   if %current3_midi -lt 80	{	if %diff3 -lt 3	   {	      if %diff3 -gt -3	      {	      10.5.1.10 9700 :vel 132 0 	      10.5.1.10 9700 :vmode 132 rev	      }	   }	}   if %current3_midi -lt 80   {   if %diff3 -le -15      {      10.5.1.10 9700 :vel 132 200000      10.5.1.10 9700 :vel 04 500000      10.5.1.10 9700 :vmode 132 rev      10.5.1.10 9700 :vmode 04 rev      }   }   if %current3_midi -lt 80   {   if %diff3 -gt -15      {         if %diff3 -le -7         {         10.5.1.10 9700 :vel 132 100000         10.5.1.10 9700 :vel 04 250000         10.5.1.10 9700 :vmode 132 rev         10.5.1.10 9700 :vmode 04 rev        }      }   }if %current3_midi -lt 80   {   if %diff3 -gt -7      {         if %diff3 -le -3         {         10.5.1.10 9700 :vel 132 50000         10.5.1.10 9700 :vel 04 125000         10.5.1.10 9700 :vmode 132 rev         10.5.1.10 9700 :vmode 04 rev        }      }    }CONFIG