/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1 

 Description
   This is the sample for writing event checkers along with the event 
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.
   
 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 13:36 jec     initial version
****************************************************************************/

#include "ES_Configure.h"
#include "ES_Events.h"
#include "ES_PostList.h"
#include "ES_ServiceHeaders.h"
#include "ES_Port.h"
#include "EventCheckers.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "TestHarnessService0.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"
#include "ES_ShortTimer.h"

#include "Hardware.h"
#include "ADMulti.h"
#include "Constants.h"
#include "DogRXSM.h"
#include "DogMasterSM.h"





bool Check4Keystroke(void)
{
  if ( IsNewKeyReady() ) // new key waiting?
  {
    ES_Event ThisEvent;
		ES_Event ReturnEvent;
    ThisEvent.EventType = ES_NEW_KEY;
    ThisEvent.EventParam = GetNewKey();
    // test distribution list functionality by sending the 'L' key out via
    // a distribution list.
		printf("Button Press: %c\r\n",ThisEvent.EventParam);
    if ( ThisEvent.EventParam == '0')
		{
			ReturnEvent.EventType = ES_LIFT_FAN_ON;
			PostHardwareTestService(ReturnEvent);
    }
		
if ( ThisEvent.EventParam == '1')
		{
			ReturnEvent.EventType = ES_LIFT_FAN_OFF;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == '3')
		{
			ReturnEvent.EventType = ES_THRUST_FAN_ON;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == '4')
		{
			ReturnEvent.EventType = ES_THRUST_FAN_OFF;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == '5')
		{
			ReturnEvent.EventType = ES_THRUST_FAN_INCR;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == '6')
		{
			ReturnEvent.EventType = ES_THRUST_FAN_DECR;
			PostHardwareTestService(ReturnEvent);
    }
    return true;
		
		if ( ThisEvent.EventParam == '7')
		{
			ReturnEvent.EventType = ES_LEFT_SERVO_UP;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == '8')
		{
			ReturnEvent.EventType = ES_LEFT_SERVO_DOWN;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == '9')
		{
			ReturnEvent.EventType = ES_LEFT_SERVO_INCR;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'A')
		{
			ReturnEvent.EventType = ES_LEFT_SERVO_DECR;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'B')
		{
			ReturnEvent.EventType = ES_RIGHT_SERVO_UP;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'C')
		{
			ReturnEvent.EventType = ES_RIGHT_SERVO_DOWN;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'D')
		{
			ReturnEvent.EventType = ES_RIGHT_SERVO_INCR;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'E')
		{
			ReturnEvent.EventType = ES_RIGHT_SERVO_DECR;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'F')
		{
			ReturnEvent.EventType = ES_BRAKES_UP;
			PostHardwareTestService(ReturnEvent);
    }
		
		if ( ThisEvent.EventParam == 'G')
		{
			ReturnEvent.EventType = ES_BRAKES_DOWN;
			PostHardwareTestService(ReturnEvent);
    }
		
  }
  return false;
}

