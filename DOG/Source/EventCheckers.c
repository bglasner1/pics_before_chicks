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
    if ( ThisEvent.EventParam == 'B'){
			ReturnEvent.EventType = ES_BYTE_RECEIVED;
			PostDogRXSM(ReturnEvent);
    }else if(ThisEvent.EventParam == 'T'){
			enableTransmit();
		}else{   // otherwise post to Service 0 for processing
    }
    return true;
  }
  return false;
}