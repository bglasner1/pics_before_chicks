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

#include "L_ButtonDebounce.h"
#include "Constants.h"

static uint8_t MyPriority;
static L_DBState_t CurrentState;

bool InitL_ButtonDebounce(uint8_t Priority) {
	// Set service priority
	MyPriority=Priority;
	// Initialize the current state machine state
	CurrentState = L_Debouncing;
	// Initialize the debouncing timer
	ES_Timer_InitTimer(L_DEBOUNCE_TIMER, DEBOUNCE_TIME);
	// End Initialization
	ES_Event ThisEvent;
	ThisEvent.EventType = ES_INIT;
	return (ES_PostToService(MyPriority, ThisEvent));
}

bool PostL_ButtonDebounce(ES_Event ThisEvent) {
	return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event RunL_ButtonDebounce(ES_Event ThisEvent) {
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT;
	
	switch (CurrentState) 
	{
		case (L_Debouncing):
		{
			if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == L_DEBOUNCE_TIMER)) 
			{
				CurrentState = L_Ready2Sample;
			}
			break;
		}
			
		case (L_Ready2Sample):
		{
			if (ThisEvent.EventType == ES_BUTTON_UP) 
			{
				ES_Timer_InitTimer(L_DEBOUNCE_TIMER, DEBOUNCE_TIME);
				CurrentState = L_Debouncing;
				ES_Event Event2Post;
				Event2Post.EventType = ES_L_BUTTON_UP;
				//POST
				printf("L Button Up\r\n");
			}
			else if (ThisEvent.EventType == ES_BUTTON_DOWN) 
			{
				ES_Timer_InitTimer(L_DEBOUNCE_TIMER, DEBOUNCE_TIME);
				CurrentState = L_Debouncing;
				ES_Event Event2Post;
				Event2Post.EventType = ES_L_BUTTON_DOWN;
				//POST
				printf("L Button Down\r\n");
			}
			break;
		}
	}
	return ReturnEvent;
}
