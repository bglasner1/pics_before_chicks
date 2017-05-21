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

}

uint8_t getDogTag( void ){
	return HARD_CODE_DOG_TAG;
}