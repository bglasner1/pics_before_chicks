/****************************************************************************
 Module
   FarmerMasterSM.c

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
#include "FarmerMasterSM.h"
#include "FarmerTXSM.h"
#include "FarmerRXSM.h"
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

static FarmerMasterState_t CurrentState;
static uint8_t MyPriority;
static uint8_t DogSelect;



bool InitFarmerMasterSM(uint8_t Priority)
{
	// state is unpaired
	CurrentState = Unpaired;
	// post entry event to self
	ES_Event EntryEvent;
	EntryEvent.EventType = ES_ENTRY;
	// set priority
	MyPriority = Priority;
	
	if (PostFarmerMasterSM(EntryEvent))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool PostFarmerMasterSM(ES_Event ThisEvent)
{
	// post event
	return ES_PostToService(MyPriority, ThisEvent);
}

ES_Event RunFarmerMasterSM(ES_Event ThisEvent)
{
	ES_Event ReturnEvent;
	FarmerMasterState NextState;
	
	// set return event
	ReturnEvent.EventType = ES_NO_EVENT;
	
	// next state is current state
	NextState = CurrentState;
	
	// switch through states
	switch(CurrentState)
	{
		
		// if current state is unpaired
		case Unpaired:
			// if event is entry
			if(ThisEvent.EventType == ES_ENTRY)
			{
				// set the LED blink timer
				// set blinker
				// call LED function
			}
			// else if event is timeout
			else if(ThisEvent.EventType == ES_TIMEOUT)
			{
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ENTRY_EVENT;
				PostFarmerMasterSM(NewEvent);
			}
			// else if event is right button down
			else if(ThisEvent.EventType == ES_R_BUTTON_DOWN)
			{
				// increment the DOG selector
				//TODO:This gives 0,1,2, but we want 1,2,3 FIX LATER
				DogSelect = (DogSelect+1)%3
			}
			// else if the event is speech detected
			else if(ThisEvent.EventType == ES_SPEECH_DETECTED)
			{
				// set request pair in FARMER_TX_SM with DOG
				setFarmerDataHeader(REQ_2_PAIR);
				// set DogTag in FarmerTXSM
				setDogTag(DogSelect);
				// Set destination address to BROADCAST since we are trying to talk to everybody
				setDestDogAddress(BROADCAST,BROADCAST); //TODO: replace this with our xbee address so we dont piss off other teams
				
				
				// TODO: set desired dog in FARMER_RX_SM ?? What do you do here?
				
				
				// next state is Wait2Pair
				NextState = Wait2Pair;
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostFarmerMasterSM(NewEvent);
				// enable transmit in FarmerTX
				enableTransmit();
			}
			break;
		// else if current state is Wait2Pair
		case Wait2Pair:
			// if event is entry
			if(ThisEvent.EventType == ES_ENTRY)
			{
				// set the LED blink timer
				// toggle the Blink LED
			}
			// else if event is timeout
			else if(ThisEvent.EventType == ES_TIMEOUT)
			{
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostFarmerMasterSM(NewEvent);
			}
			// else if event is Lost connection
			else if(ThisEvent.EventType == ES_LOST_CONNECTION)
			{
				// disable transmit in FarmerTX
				disableTransmit();
				// next state is Unpaired
				NextState = Unpaired;
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostFarmerMasterSM(NewEvent);
			}
			// else if event is ES_CONNECTION_SUCCESSFUL
			else if(ThisEvent.EventType == ES_CONNECTION_SUCCESSFUL)
			{
				//TODO:
				// clear blinker
				// Call LED function
				
				//***********I DONT BELIEVE WE WANT TO DO THE 2 LINES BELOW*******
				// set paired in TX and RX
				//setPair();
				//*****************************************************************
				
				
				// Set message to ENCR_KEY in FarmerTx
				setFarmerDataHeader(ENCR_KEY);
				// Next state is Wait2Encrypt
				NextState = Wait2Encrypt;
			}
			break;
			
		// else if current state is Wait2Encrypt
		case Wait2Encrypt:
			// if event is ES_PAIR_SUCCESSFUL
			if(ThisEvent.EventType == ES_PAIR_SUCCESSFUL)
			{
				// next state is Paired
				NextState = Paired;
				// Set message to CTRL
				setFarmerDataHeader(CTRL
				
				//TODO:
				// clear blinker
				// Call LED function
				
			
				// set paired in TX and RX
				setPair();
			// else if event is Lost connection
			if(ThisEvent.EventType == ES_LOST_CONNECTION)
			{
				// next state is Unpaired
				NextState = Unpaired;
				// disable transmit in FarmerTX
				disableTransmit();
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostFarmerMasterSM(NewEvent);
			}
			break;
			
		// else if state is paired
		case Paired:
			// if event is right button down
			if(ThisEvent.EventType == ES_R_BUTTON_DOWN)
			{
				// set right brake active in TX
				EnableRightBrake();
			}
			// else if event is right button up
			else if(ThisEvent.EventType == ES_R_BUTTON_UP)
			{
				// set right brake inactive in TX
				DisableRightBrake();
			}
			// else if event is left button down
			else if(ThisEvent.EventType == ES_L_BUTTON_DOWN)
			{
				// set left brake active in TX
				EnableLeftBrake();
			}
			// else if event is left button up
			else if (ThisEvent.EventType == ES_L_BUTTON_UP)
			{
				// set left brake inactive in TX
				DisableLeftBrake();
			}
			// else if event is reverse button down
			else if(ThisEvent.EventType == ES_REVERSE_BUTTON_DOWN)
			{
				// set reverse active in TX
				EnableReverse();
			}
			// else if event is reverse button up
			else if(ThisEvent.EventType == ES_REVERSE_BUTTON_UP)
			{
				// set forward active in TX
				DisableReverse();
			}
			// else if event is peripheral button down
			else if(ThisEvent.EventType == ES_P_BUTTON_DOWN)
			{
				// toggle peripheral in tx
				TogglePeripheral();
			}
			// else if event is ES_RESEND_ENCRYPT
			else if(ThisEvent.EventType == ES_RESEND_ENCRYPT)
			{
				// Next state is Wait2Encrypt
				NextState = Wait2Encrypt;
				// Set message to ENCR_KEY in FarmerTX
				setFarmerDataHeader(ENCR_KEY);
			}
			// else if event is lost connection
			else if(ThisEvent.EventType == ES_LOST_CONNECTION)
			{
				// set unpaired in TX and RX
				setUnpair();
				// disable transmit in FarmerTX
				disableTransmit();
				// post entry event to self
				ES_Event NewEvent;
				NewEvent.EventType = ES_ENTRY;
				PostFarmerMasterSM(NewEvent);
				// next state is unpaired
				NextState = Unpaired;
			}
			break;
	} //end switch	
	
	//Set current state to next state
	CurrentState = NextState;
	//return return event
	return ReturnEvent;
		
}

static void LED_Setter(void)
{
	// Clear last LED that was written
	// if blinker is set
		// current LED is Yellow LED associated with the DOG selector
		// toggle Last LED state
		// write Last LED state to selected LED 
	// else
		// current LED is Green LED assosciated with the DOG selector
		// clear last LED state
		// write selected LED High
	// last LED is current LED
}