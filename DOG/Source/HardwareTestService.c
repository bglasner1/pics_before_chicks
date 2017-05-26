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
static uint16_t LeftServoUp;
static uint16_t LeftServoDown;
static uint16_t RightServoUp;
static uint16_t RightServoDown;



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
	LeftServoUp = 300;
	RightServoUp = 300;
	LeftServoDown = 1600;
	RightServoDown = 1600;
	SetLeftBrakePosition(LeftServoDown);
	SetRightBrakePosition(RightServoDown);

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
			printf("ES_LIFT_FAN_ON\r\n");
			sendToPIC(25);
    }
		
		else if ( ThisEvent.EventType == ES_LIFT_FAN_OFF)
		{
			printf("ES_LIFT_FAN_OFF\r\n");
			sendToPIC(0);
			SetThrustFan(127);
    }
		
		else if ( ThisEvent.EventType == ES_THRUST_FAN_ON)
		{
			printf("ES_THRUST_FAN_ON\r\n");
			printf("RUN HARDWARE TEST -- DRIVE CTRL = %i\r\n", DriveCtrl);

			SetThrustFan(DriveCtrl);
    }
		
		if ( ThisEvent.EventType == ES_THRUST_FAN_OFF)
		{
			printf("ES_THRUST_FAN_OFF\r\n");
			printf("RUN HARDWARE TEST -- DRIVE CTRL = %i\r\n", 127);
			SetThrustFan(127);
    }
		
		if ( ThisEvent.EventType == ES_THRUST_FAN_INCR)
		{
			printf("ES_THRUST_FAN_INCR\r\n");
			if(DriveCtrl > 250)
			{
				DriveCtrl = 255;
			}
			else
			{
				DriveCtrl = DriveCtrl + 5;
			}
			
			printf("RUN HARDWARE TEST -- DRIVE CTRL = %i\r\n", DriveCtrl);
			SetThrustFan(DriveCtrl);
    }
		
		if ( ThisEvent.EventType == ES_THRUST_FAN_DECR)
		{
			printf("ES_THRUST_FAN_DECR\r\n");
			if(DriveCtrl < 5)
			{
				DriveCtrl = 0;
			}
			else
			{
				DriveCtrl = DriveCtrl - 5;
			}
			
			printf("RUN HARDWARE TEST -- DRIVE CTRL = %i\r\n", DriveCtrl);
			SetThrustFan(DriveCtrl);
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_UP)
		{
			printf("ES_LEFT_SERVO_UP\r\n");
			SetLeftBrakePosition(LeftServoUp);
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_DOWN)
		{
			printf("ES_LEFT_SERVO_DOWN\r\n");
			SetLeftBrakePosition(LeftServoDown);
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_UP_INCR)
		{
			LeftServoUp = LeftServoUp + 10;
			if(LeftServoUp > 3000)
			{
				LeftServoUp = 3000;
				printf("LEFT SERVO UP IS AT 3000 LIMIT\r\n");
			}
			SetLeftBrakePosition(LeftServoUp);
			printf("NEW LEFT SERVO UP POSITION = %d\r\n", LeftServoUp);
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_UP_DECR)
		{
			if(LeftServoUp >= 10)
			{
				LeftServoUp = LeftServoUp - 10;
			}
			else
			{
				LeftServoUp = 0;
				printf("LEFT SERVO UP IS AT 0 LIMIT\r\n");
			}
			SetLeftBrakePosition(LeftServoUp);
			printf("NEW LEFT SERVO UP POSITION = %d\r\n", LeftServoUp);
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_DOWN_INCR)
		{
			LeftServoDown = LeftServoDown + 10;
			if(LeftServoDown > 3000)
			{
				LeftServoDown = 3000;
				printf("LEFT SERVO DOWN IS AT 3000 LIMIT\r\n");
			}
			SetLeftBrakePosition(LeftServoDown);
			printf("NEW LEFT SERVO DOWN POSITION = %d\r\n", LeftServoDown);			
    }
		
		if ( ThisEvent.EventType == ES_LEFT_SERVO_DOWN_DECR)
		{
			if(LeftServoDown >= 10)
			{
				LeftServoDown = LeftServoDown - 10;
			}
			else
			{
				LeftServoDown = 0;
				printf("LEFT SERVO DOWN IS AT 0 LIMIT\r\n");
			}
			SetLeftBrakePosition(LeftServoDown);
			printf("NEW LEFT SERVO DOWN POSITION = %d\r\n", LeftServoDown);
    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_UP)
		{
			printf("ES_RIGHT_SERVO_UP\r\n");
			SetRightBrakePosition(RightServoUp);
    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_DOWN)
		{
			printf("ES_RIGHT_SERVO_DOWN\r\n");
			SetRightBrakePosition(RightServoDown);
    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_UP_INCR)
		{
			RightServoUp = RightServoUp + 10;
			if(RightServoUp > 3000)
			{
				RightServoUp = 3000;
				printf("RIGHT SERVO UP IS AT 3000 LIMIT\r\n");
			}
			SetRightBrakePosition(RightServoUp);
			printf("NEW RIGHT SERVO UP POSITION = %d\r\n", RightServoUp);
    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_UP_DECR)
		{
			if(RightServoUp >= 10)
			{
				RightServoUp = RightServoUp - 10;
			}
			else
			{
				RightServoUp = 0;
				printf("RIGHT SERVO UP IS AT 0 LIMIT\r\n");
			}
			SetRightBrakePosition(RightServoUp);
			printf("NEW RIGHT SERVO UP POSITION = %d\r\n", RightServoUp);
    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_DOWN_INCR)
		{
			RightServoDown = RightServoDown + 10;
			if(RightServoDown > 3000)
			{
				RightServoDown = 3000;
				printf("RIGHT SERVO DOWN IS AT 3000 LIMIT\r\n");
			}
			SetRightBrakePosition(RightServoDown);
			printf("NEW RIGHT SERVO DOWN POSITION = %d\r\n", RightServoDown);
    }
		
		if ( ThisEvent.EventType == ES_RIGHT_SERVO_DOWN_DECR)
		{
			if(RightServoDown >= 10)
			{
				RightServoDown = RightServoDown - 10;
			}
			else
			{
				RightServoDown = 0;
				printf("RIGHT SERVO DOWN IS AT 0 LIMIT\r\n");
			}
			SetRightBrakePosition(RightServoDown);
			printf("NEW RIGHT SERVO DOWN POSITION = %d\r\n", RightServoDown);
    }
		
		if ( ThisEvent.EventType == ES_BRAKES_UP)
		{
			printf("ES_BRAKES_UP\r\n");
			SetLeftBrakePosition(LeftServoUp);
			SetRightBrakePosition(RightServoUp);
    }
		
		if ( ThisEvent.EventType == ES_BRAKES_DOWN)
		{
			printf("ES_BRAKES_DOWN\r\n");
			SetLeftBrakePosition(LeftServoDown);
			SetRightBrakePosition(RightServoDown);
    }
	

  return ReturnEvent;
}


/***************************************************************************
 private functions
 ***************************************************************************/

