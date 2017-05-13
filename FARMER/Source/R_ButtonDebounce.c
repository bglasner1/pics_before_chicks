#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

#include "R_ButtonDebounce.h"
#include "L_ButtonDebounce.h"
#include "Constants.h"

static uint8_t MyPriority;
static R_DBState_t CurrentState;

bool InitR_ButtonDebounce(uint8_t Priority) {
	// Set service priority
	MyPriority=Priority;
	// Initialize the current state machine state
	CurrentState = R_Debouncing;
	// Initialize the debouncing timer
	ES_Timer_InitTimer(R_DEBOUNCE_TIMER, DEBOUNCE_TIME);
	// End Initialization
	ES_Event ThisEvent;
	ThisEvent.EventType = ES_INIT;
	return (ES_PostToService(MyPriority, ThisEvent));
}

bool PostR_ButtonDebounce(ES_Event ThisEvent) {
	return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event RunR_ButtonDebounce(ES_Event ThisEvent) {
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT;
	
	switch (CurrentState) 
	{
		case (R_Debouncing):
		{
			if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == R_DEBOUNCE_TIMER)) 
			{
				CurrentState = R_Ready2Sample;
			}
			break;
		}
			
		case (R_Ready2Sample):
		{
			if (ThisEvent.EventType == ES_BUTTON_UP) 
			{
				ES_Timer_InitTimer(R_DEBOUNCE_TIMER, DEBOUNCE_TIME);
				CurrentState = R_Debouncing;
				ES_Event Event2Post;
				Event2Post.EventType = ES_R_BUTTON_UP;
				//POST
			}
			else if (ThisEvent.EventType == ES_BUTTON_DOWN) 
			{
				ES_Timer_InitTimer(R_DEBOUNCE_TIMER, DEBOUNCE_TIME);
				CurrentState = R_Debouncing;
				ES_Event Event2Post;
				Event2Post.EventType = ES_R_BUTTON_DOWN;
				//POST
			}
			break;
		}
	}
	return ReturnEvent;
}
