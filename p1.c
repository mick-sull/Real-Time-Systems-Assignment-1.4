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

	struct MemData *CommonMem;
	struct waterData *WaterData;

	ptrMemName = MEMORY_NAME;
	ptrMemWater = MEMORY_WATER;



	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);


	SleepTime = 100;
	_os_sleep(&SleepTime, &DummySignal);


	TimeToDelay = 3000;
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
			 

				/*increment tank level to make it work like a sensor data being read in
				  *if the valve is open(1) decrement the water level value water is being released
				  *if the valve is closed(0) increment the water level value as water is not being released
				
				*/

				if(WaterData -> tankValve == 1){
					WaterData -> tankLevelHight = 	WaterData -> tankLevelHight -1;
				}
			  	if(WaterData -> tankValve == 0)
				{
					WaterData -> tankLevelHight = 	WaterData -> tankLevelHight+1;
				}


				printf ("P1: Tank Level Hight = %d \n",  WaterData -> tankLevelHight);
				
				
				
				if(WaterData -> tankLevelHight > 90)
				{
				  _os_send(CommonMem->PID[7], 400);
				}
				if(WaterData -> tankLevelHight < 50){
				  _os_send(CommonMem->PID[7], 500);
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
			}
		  else
			printf("P1: The signal value which caused the alarm is %d\n", ReceivedSginal);
			 
		
	}
	
}

