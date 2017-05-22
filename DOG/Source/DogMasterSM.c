/****************************************************************************
 Module
   DogMasterSM.c

 Revision
   1.0.1

 Description
   The receiving state machine for the Farmer

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 05/20/17 1:51	bag			created for the project
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "DogMasterSM.h"
#include "DogTXSM.h"
#include "DogRXSM.h"
#include "Constants.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "inc/hw_uart.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#define HARD_CODE_DOG_TAG 101

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine 
*/   

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file

static DogMasterState_t CurrentState;
static uint8_t MyPriority;
static uint8_t DogSelect;


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDogMasterSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     Matthew W Miller, 5/13/2017, 17:31
****************************************************************************/
bool InitDogMasterSM(uint8_t Priority)
{
	// state is unpaired
	CurrentState = Unpaired;
	// post entry event to self
	ES_Event EntryEvent;
	EntryEvent.EventType = ES_ENTRY;
	// set priority
	MyPriority = Priority;
	
	if (PostDogMasterSM(EntryEvent))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/****************************************************************************
 Function
     PostDogMasterSM

 Parameters
     EF_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostDogMasterSM(ES_Event ThisEvent)
{
	// post event
	return ES_PostToService(MyPriority, ThisEvent);
}


/****************************************************************************
 Function
    RunDogMasterSM

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   Matthew Miller, 05/13/17, 17:54
****************************************************************************/
ES_Event RunDogMasterSM(ES_Event ThisEvent)
{
	// set return event
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT;
	
	// next state is current state
	DogMasterState_t NextState;
	NextState = CurrentState;
	printf("DogMasterCurrentState = %i\r\n",CurrentState);
	
	// switch through states
	switch(CurrentState)
	{
		// if current state is unpaired
		case Unpaired:
			// if event is entry
		printf("Dog Master SM -- Unpaired State -- Top\r\n");
			if(ThisEvent.EventType == ES_ENTRY)
			{
				// stop electromechanical indicator
				// clear LED active
				// call LED setter
				// turn thrust fan off
				// set all brakes inactive
				// call brake setter
				// turn lift fan off
				//printf("Dog Master SM -- Unpaired State -- Entry Event\r\n");
			}
			
			// else if the event is broadcast detected
			else if(ThisEvent.EventType == ES_BROADCAST_RECEIVED)
			{
				// next state is Wait2Pair
				NextState = Wait2Pair;
				//printf("Dog Master SM -- Unpaired State -- Broadcast Received\r\n");
			}
			
			break;
			
		// else if current state is Wait2Pair
		case Wait2Pair:
		//printf("Dog Master SM -- Wait2Pair State -- Top\r\n");
			//if event is Lost connection
			if(ThisEvent.EventType == ES_LOST_CONNECTION)
			{
				//printf("Dog Master SM -- Wait2Pair State -- Connection Lost\r\n");
				// next state is Unpaired
				NextState = Unpaired;
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostDogMasterSM(NewEvent);
			}
			
			// else if event is pair successful
			else if(ThisEvent.EventType == ES_PAIR_SUCCESSFUL)
			{
				printf("Dog Master SM -- Wait2Pair State -- Successful Pair\r\n");
				// set LED active
				// Call LED setter
				// turn on electromechanical indicator
				// start lift fan
				// next state is Paired
				NextState = Paired;
				//start lift fan
			}
			
			break;
			
		// else if state is paired
		case Paired:
		printf("Dog Master SM -- Paired State -- Top\r\n");
			// if event is thrust
				// determine thrust setting and direction
				// set direction on thrust fan
				// set thrust value on thrust fan
			// else if event is start lift fan
				// set lift fan active
				// call PIC commander
			// else if event is stop lift fan
				// set lift fan inactive
				// call PIC commander
			// else if event is brake
				// set brakes active
				// call brake setter
			// else if event reset brakes
				// set brakes inactive
				// call brake setter
			// else if event is turn right
				// set right brake active
				// call brake setter
			// else if event is reset right
				// set right brake inactive
				// call brake setter
			// else if event is turn left
				// set left brake active
				// call brake setter
			// else if event is reset left
				// set left brake inactive
				// call brake setter
			// else if event is blinker timeout
				// update LED pattern
				// call LED setter
				// set blink timer
			// else if event is lost connection
			if(ThisEvent.EventType == ES_LOST_CONNECTION)
			{
				//printf("Dog Master SM -- Paired State -- Lost Connection\r\n");
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostDogMasterSM(NewEvent);
				// next state is unpaired
				NextState = Unpaired;
			}
			break;				
	}
	CurrentState = NextState;
	return ReturnEvent;
}


static void LED_Setter(void)
{	
	// if LED inactive
		// light Red LEDs
	// else if LED active
		//light LED according to current pattern selection
}

static void Brake_Setter(void)
{
	// if Brake active
		// set left and right brakes down
	// else if Brake inactive
		// if right brake active
			// set right brake down
		// else if right brake inactive
			// raise right brake
		// if left brake active
			// set left brake down
		// else if left brake inactive
			//raise left brake
}

static void PIC_Commander(void)
{
	// insert PIC UART communication code
}

uint8_t getDogTag( void ){
	return HARD_CODE_DOG_TAG;
}