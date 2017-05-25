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


static bool BadCheckSum = false;


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
    if ( ThisEvent.EventParam == 'B')
		{
			ReturnEvent.EventType = ES_BYTE_RECEIVED;
			PostDogRXSM(ReturnEvent);
    }
		else if(ThisEvent.EventParam == 'T')
		{
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}
		else if(ThisEvent.EventParam == 'H')
		{
			sendToPIC(0x0C);
		}else if(ThisEvent.EventParam == 'D'){
			sendToPIC(0x02);
		}else if(ThisEvent.EventParam == 'A'){
			sendToPIC(0x16);
		}else if(ThisEvent.EventParam == 'E'){
			setDogDataHeader(ENCR_RESET);
			//Post transmit ENCR_RESET Event to TX_SM
			ES_Event ReturnEvent;
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'P'){
			setDogDataHeader(PAIR_ACK);
		
			//Post transmit ENCR_RESET Event to TX_SM
			ES_Event ReturnEvent;
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'S'){
			setDogDataHeader(STATUS);
			//Post transmit STATUS Event to TX_SM
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'L'){
			ReturnEvent.EventType = ES_LOST_CONNECTION;
			PostDogMasterSM(ReturnEvent);
		}else if(ThisEvent.EventParam == '1'){
			setDogDataHeader(PAIR_ACK);
			//Post transmit STATUS Event to TX_SM
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == '2'){
			setDogDataHeader(ENCR_RESET);
			//Post transmit STATUS Event to TX_SM
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == '3'){
			setDogDataHeader(STATUS);
			//Post transmit STATUS Event to TX_SM
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostDogTXSM(ReturnEvent);
		}else{   // otherwise post to Service 0 for processing
    }
    return true;
  }
  return false;
}

bool Check4BadCheckSum(void)
{
	if(BadCheckSum)
	{
		BadCheckSum = false;
		printf("------------------------------------------DogRXSM - BAD CHECKSUM ERROR\r\n");
		return true;
	}
	return false;
}

void SetBadCheckSum(void)
{
	BadCheckSum = true;
}


