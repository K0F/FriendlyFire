#!/bin/shactor -d0 -n stealth_atjog -h 10.5.1.12 -p 9850 << CONFIG10.5.1.12 9800 midi   note_on   midi_channel 3    low   0x00   hi    0x7f   low_velocity   0x00   hi_velocity    0x7f    endif %mnote -ge 0	{		if %mnote -le 45		{		10.5.1.17 9900 :at lf; at jg-200		}	}	if %mnote -ge 46	{			{		if %mnote -le 75		10.5.1.17 9900 :at st		}	}if %mnote -ge 76	{		{			if %mnote -le 127	10.5.1.17 9900 :ay lf; at jg200		}	}CONFIG