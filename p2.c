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


	/*24 values to resemble hourly air tempetures(deg C)
	*Starting at 1 am and finishing at 23.59
	*Figures are taken from Cork Airport on 16/5/2016
	*http://www.met.ie/climate/daily-data.asp
	*/
	int	reads[] = {11, 9, 9,10,9,8,9,10,11,12,13,12,13,14,15,15,15,15,16,15,14,13,11,10,9,8};
	i = 0;


	ptrMemName = MEMORY_NAME;
	ptrMemWater = MEMORY_WATER;



	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);

	SleepTime = 100;
	_os_sleep(&SleepTime, &DummySignal);

	/*3600000 is an hour
	TimeToDelay = 3600000;
	For the demo I will leave it at every 20 seconds.
	*/

	TimeToDelay = 20000;
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
					printf("Event 'EVENT_WATER_DATA' not linked?\n");
					exit(errno);
					}

				if((errno = _os_ev_wait(ev_id, &value, &signal, 1, 1)) != 0)
					{
					printf("P2: Waiting on event error\n");
					exit(0);
					}
				
			  	errno = _os_link(&ptrMemName, (mh_com**)&mod_head, (void **)&CommonMem, &type_lang, &attr_rev);	
			  	errno = _os_link(&ptrMemWater, (mh_com**)&mod_head, (void **)&WaterData, &type_lang, &attr_rev);
			 
				printf("P2: Air temperture:  %d\n", reads[i]);
				WaterData -> airTemp = reads[i]	;

				/*	If the air temp is less then 10 Deg C, turn off the 
				 * 	solar heating and use gas/oil heating instead.
				 *  The 10 Deg C is a fabricated figure to test the air temp data.
				 */

				if(WaterData -> airTemp < 10){
					/*Start reducing the water tempeture of the tank due to it 
					*not being heated by the solar system. 
					*Also turn off pump as you dont want the cold water circulating
					*around the system. This pump is just for pumping water from the solar panels.
					*I will keep measuring the tank level as the oil/gas system could still be 
					*used.
					*/
					_os_send(CommonMem->PID[3], 500); /*Turn off Pump*/
					_os_send(CommonMem->PID[4], 500); /*Reduce the water tempeture*/

				}

				if(WaterData -> airTemp >= 10){ 
					_os_send(CommonMem->PID[3], 400); /*Turn on Pump*/
					_os_send(CommonMem->PID[4], 400); /*Increase the water tempeture*/
 
			   }


				/*increment message number*/
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
					printf("Event unlink error\n");
					exit(errno);
					}

				if(	i==23)
					i=0;
				else
					i= i +1;
				
	 	}
		  else
			printf("P2: The signal value which caused the alarm is %d\n", ReceivedSginal);
			 
		
	}  
	
}

