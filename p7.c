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
		case 400 : printf("P7:Water Valve Open\n");
			chanageValveStatus(sig);
	 		break;
	 	case 500 : printf("P7:Water Valve Closed\n");
			chanageValveStatus(sig);
	 		break;					   
	   }

	_os_rte();
	}

chanageValveStatus(signal_code sig){
	mh_com mod_head;
	event_id ev_id;
	u_int32 value;
	signal_code signal;
	char *ptrMemName, *ptrMemWater;
	u_int16 attr_rev, type_lang;
	struct MemData *CommonMem;
	struct waterData *WaterData;

	ptrMemName = MEMORY_NAME;
	ptrMemWater = MEMORY_WATER;
		
	type_lang = (MT_DATA << 8);
	attr_rev = (MA_REENT << 8);
		if((errno = _os_ev_link(EVENT_WATER_DATA, &ev_id)) != 0)
		{
		printf("Event 'EVENT_WATER_DATA' not linked?\n");
		exit(errno);
		}

	if((errno = _os_ev_wait(ev_id, &value, &signal, 1, 1)) != 0)
		{
		printf("Waiting on event error\n");
		exit(0);
		}
	
 
  	errno = _os_link(&ptrMemName, (mh_com**)&mod_head, (void **)&CommonMem, &type_lang, &attr_rev);	
  	errno = _os_link(&ptrMemWater, (mh_com**)&mod_head, (void **)&WaterData, &type_lang, &attr_rev);
	
   	if(sig == 400)
	{
		   WaterData -> tankValve = 1;
	}

	if(sig == 500)
	{
		   WaterData -> tankValve = 0;
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
