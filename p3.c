#include <stdio.h>
#include <signal.h>
#include <module.h>
#include <types.h>
#include <errno.h>
#include <cglob.h>

#include "MemData.h"
#include "waterData.h"
#include "constantValues.h"

#define EVENT_NAME "SemaEvent"
#define EVENT_WATER_DATA "SemaEventWater"

#define MEMORY_NAME "CommonMem"
#define MEMORY_WATER "WaterData"
#define MEMORY_CONSTANTS "ConstantValues"


sig_handler(signal_code sig)
	{	
	switch (sig)
		{
		case 400 : printf("P3: Pump 	ON\n");
			chanageValveStatus(sig);
	 		break;
	 	case 500 : printf("P3: Pump	 OFF\n");
			chanageValveStatus(sig);
	 		break;					   
	   }
	_os_rte();
	}

chanageValveStatus(signal_code sig){
	mh_com mod_head;
	event_id ev_id;
	int finished;
	u_int32 value;
	signal_code signal;
	char *ptrMemName, *ptrMemWater, *ptrMemConstant;
	u_int16 attr_rev, type_lang;
	struct MemData *CommonMem;
	struct waterData *WaterData;
	struct waterDataConstants *ConstantValues;
	u_int32 TimeToDelay;
	u_int32 i;
	alarm_id MyAlarm;
	signal_code WakeupSignal;
	u_int32 SleepValue;
	signal_code ReceivedSginal;

	ptrMemName = MEMORY_NAME;
	ptrMemWater = MEMORY_WATER;
	ptrMemConstant = MEMORY_CONSTANTS;
				
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
		if((errno = _os_ev_link(EVENT_WATER_DATA, &ev_id)) != 0)
		{
		printf("Event 'EVENT_WATER_DATA' not linked?\n");
		exit(errno);
		}

	if((errno = _os_ev_wait(ev_id, &value, &signal, 1, 1)) != 0)
		{
		printf("P3: Waiting on event error\n");
		exit(0);
		}
	
  	errno = _os_link(&ptrMemName, (mh_com**)&mod_head, (void **)&CommonMem, &type_lang, &attr_rev);	
  	errno = _os_link(&ptrMemWater, (mh_com**)&mod_head, (void **)&WaterData, &type_lang, &attr_rev);
	errno = _os_link(&ptrMemConstant, (mh_com**)&mod_head, (void **)&ConstantValues, &type_lang, &attr_rev);
	 
   	if(sig == 400)
	{
		/*printf("P3: Pump running....\n");*/
		WaterData -> pumpSpeed = 50;
		 		
	}

	if(sig == 500)
	{
		 /*printf("P3: Pump stopped....\n");*/
		 WaterData -> pumpSpeed = 0;
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

main()
	{
	error_code err;
	signal_code DummySignal;
	u_int32 SleepTime;


	if ((err = _os_intercept(sig_handler, _glob_data)) != 0)
		exit(err);

	SleepTime = 100;
	_os_sleep(&SleepTime, &DummySignal);
	
	
	SleepTime = 0;
	while(1) {
		_os_sleep(&SleepTime, &DummySignal);
		}


	}
