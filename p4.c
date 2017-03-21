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


sig_handler(signal_code sig)
	{	
	switch (sig)
		{
		case 500 : printf("P4: Water cooling down...\n");
	 		break;
		
		case 400 : printf("P4: Water heating up...\n");
		break;				   
	   }
	_os_rte();
	}
	
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

	ptrMemName = MEMORY_NAME;
	ptrMemWater = MEMORY_WATER;


	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);


	if ((err = _os_intercept(sig_handler, _glob_data)) != 0)
		exit(err);

	SleepTime = 100;
	_os_sleep(&SleepTime, &DummySignal);


	TimeToDelay = 2500;
	WakeupSignal = 1; 
   
	if ((errno = _os_alarm_cycle(&MyAlarm, WakeupSignal, TimeToDelay)) != 0)
		{
			printf("P4: error creating alarm\n");
		}

	SleepValue = 0; /* Infinite loop, sleep forever */
     	
		
	while (1){
		_os_sleep(&SleepValue, &ReceivedSginal);
		if(ReceivedSginal ==0){

				if((errno = _os_ev_link(EVENT_WATER_DATA, &ev_id)) != 0)
					{
					printf("Event 'SemaEvent' not linked?\n");
					exit(errno);
					}

				if((errno = _os_ev_wait(ev_id, &value, &signal, 1, 1)) != 0)
					{
					printf("P4: Waiting on event error\n");
					exit(0);
					}

			  	errno = _os_link(&ptrMemName, (mh_com**)&mod_head, (void **)&CommonMem, &type_lang, &attr_rev);	
			  	errno = _os_link(&ptrMemWater, (mh_com**)&mod_head, (void **)&WaterData, &type_lang, &attr_rev);
			 
			   	if(WaterData -> pumpSpeed == 0){
					/*pump is shut off*/
					/*I dont want the water to go into a minus figure as it	
					 * it wouldnt happen in a real water tank in a house
					 */
					if (WaterData -> tankTemp != 0 && WaterData -> pumpSpeed != 0){						
						WaterData -> tankTemp = WaterData -> tankTemp -1;
						printf ("P4: Water Tank Tempeture = %d C \n",  WaterData -> tankTemp);
					}				

				}

			   	if(WaterData -> pumpSpeed > 0){/*pump is turned on*/
					if (WaterData -> tankTemp != 0 && WaterData -> pumpSpeed > 0){						
						WaterData -> tankTemp = WaterData -> tankTemp +1;
						printf ("P4: Water Tank Tempeture = %d C \n",  WaterData -> tankTemp);
					}				

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
			printf("P4: The signal value which caused the alarm is %d\n", ReceivedSginal);	
	}
	
}

