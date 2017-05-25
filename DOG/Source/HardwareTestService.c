/****************************************************************************
 Module
   TemplateFSM.c

 Revision
   1.0.1

 Description
   This is a template file for implementing flat state machines under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "HardwareTestService.h"
#include "Hardware.h"

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file


// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;
static uint8_t DriveCtrl;


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
bool InitHardwareTestService ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;
	DriveCtrl = 127;

  // post the initial transition event
  if (ES_PostToService( MyPriority, ThisEvent) == true)
  {
      return true;
  }else
  {
      return false;
  }
}

/****************************************************************************
 Function
     PostTemplateFSM

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
bool PostHardwareTestService( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunHardwareTestService

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event RunHardwareTestService( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
	
  ReturnEvent.EventType = ES_NO_EVENT; // assume no 
	
		if ( ThisEvent.EventType == ES_LIFT_FAN_ON)
		{
			sendToPIC(25);
    }
		
		else if ( ThisEvent.EventType == ES_LIFT_FAN_OFF)
		{
			sendToPIC(0);
    }
		
		else if ( ThisEvent.EventType == ES_THRUST_FAN_ON)
		{
			SetThrustFan(DriveCtrl);
    }
		
		if ( ThisEvent.EventType == ES_THRUST_FAN_OFF)
		{
			SetThrustFan(DriveCtrl);
    }
		
		if ( ThisEvent.EventType == ES_THRUST_FAN_INCR)
		{

    }
		
		if ( ThisEvent.EventType == ES_THRUST_FAN_DECR)
		{

    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_UP)
		{

    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_DOWN)
		{
			
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_INCR)
		{

    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_DECR)
		{

    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_UP)
		{

    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_DOWN)
		{

    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_INCR)
		{

    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_DECR)
		{

    }
		
		if ( ThisEvent.EventType == ES_BRAKES_UP)
		{

    }
		
		if ( ThisEvent.EventType == ES_BRAKES_DOWN)
		{

    }

  return ReturnEvent;
}


/***************************************************************************
 private functions
 ***************************************************************************/

