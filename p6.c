#include <stdio.h>
#include <signal.h>
#include <module.h>
#include <types.h>
#include <errno.h>

#include <cglob.h>

#include "MemData.h"
#include "waterData.h"

#define EVENT_NAME "SemaEvent"
#define EVENT_WATER_DATA "SemaEventWater"

#define MEMORY_NAME "CommonMem"
#define MEMORY_WATER "WaterData"



main()
	{
	signal_code DummySignal;
	u_int32 SleepTime;
	mh_com mod_head;
	event_id ev_id;
	u_int32 value;
	signal_code signal;
	char *ptrMemName, *ptrMemWater;
	error_code err;
	u_int16 attr_rev, type_lang;

	signal_code ReceivedSginal;
	u_int32 SleepValue;

	alarm_id MyAlarm;
	signal_code WakeupSignal;

	u_int32 TimeToDelay;
	u_int32 i;


	struct MemData *CommonMem;
	struct waterData *WaterData;

	/* These values will represent the angle of solar panel.
	*  This will give the optimum angle to get the best out of the system.  
	*  The values are monthly based. The values are availble on the url below.
	*  http://solarelectricityhandbook.com/solar-angle-calculator.html
	*  The values are for Cork.
	*/
	
	int	reads[] = {22,30,38,46,54,62,54,46,38,30,22,14};
	i = 0;


	ptrMemName = MEMORY_NAME;
	ptrMemWater = MEMORY_WATER;



	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);



	SleepTime = 100;
	_os_sleep(&SleepTime, &DummySignal);

	/*2629746000 is 1 month in milliseonds
	TimeToDelay = 2629746000;
	For the demo I will leave it at every 60 seconds.
	*/

	TimeToDelay = 60000;
	WakeupSignal = 1; 
   
	if ((errno = _os_alarm_cycle(&MyAlarm, WakeupSignal, TimeToDelay)) != 0)
		{
			printf("error creating alarm\n");
		}

	SleepValue = 0; /* Infinite loop, sleep forever */
     	
		
   	while (1){
		_os_sleep(&SleepValue, &ReceivedSginal);
		if(ReceivedSginal ==0){	 

				if((errno = _os_ev_link(EVENT_WATER_DATA, &ev_id)) != 0)
					{
					printf("P6: Event 'SemaEvent' not linked?\n");
					exit(errno);
					}

				if((errno = _os_ev_wait(ev_id, &value, &signal, 1, 1)) != 0)
					{
					printf("P6: Waiting on event error\n");
					exit(0);
					}
				
			  	errno = _os_link(&ptrMemName, (mh_com**)&mod_head, (void **)&CommonMem, &type_lang, &attr_rev);	
			  	errno = _os_link(&ptrMemWater, (mh_com**)&mod_head, (void **)&WaterData, &type_lang, &attr_rev);
			 

				WaterData -> solarPanelTilt = reads[i];

				printf("P6: Solar Panel Tilt:  %d degree angle\n", reads[i]);
				WaterData -> airTemp = reads[i]	;


				/*increment message number	  */
				WaterData -> MessageNumbers = WaterData -> MessageNumbers +1 ;
							
				/* Exit from the critical section, therefore the semaphore event is realesed.  */
				if((errno = _os_ev_signal(ev_id, &value, 0)) != 0)
					{
					printf("Signalling event error\n");
					exit(errno);
					}

				/* Unlink from the semaphore evet. */
				if((errno = _os_ev_unlink(ev_id)) != 0)
					{
					printf("P6: Event unlink error\n");
					exit(errno);
					}

				if(	i==11)
					i=0;
				else
					i= i +1;
				
	 	}
		  else
			printf("P6: The signal value which caused the alarm is %d\n", ReceivedSginal);
			 
		
	}  
	
}

