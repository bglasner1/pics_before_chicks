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
#include "R_ButtonDebounce.h"
#include "L_ButtonDebounce.h"
#include "Reverse_ButtonDebounce.h"
#include "Peripheral_ButtonDebounce.h"
#include "FarmerRXSM.h"
#include "FarmerTXSM.h"
#include "FarmerMasterSM.h"


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
			PostFarmerRXSM(ReturnEvent);
    }else if(ThisEvent.EventParam == 'T'){
			ReturnEvent.EventType = ES_TIMEOUT;
			PostFarmerMasterSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'P'){
			setPair();
		}else if(ThisEvent.EventParam == 'U'){
			setUnpair();
		}else if(ThisEvent.EventParam == 'S'){
			printf("Speaker Button Press\r\n");
			ReturnEvent.EventType = ES_SPEECH_DETECTED;
			PostFarmerMasterSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'C'){
			ReturnEvent.EventType = ES_CONNECTION_SUCCESSFUL;
			PostFarmerMasterSM(ReturnEvent);
			printf("Connection Success button press\r\n");
		}else if(ThisEvent.EventParam == 'K'){
			ReturnEvent.EventType = ES_PAIR_SUCCESSFUL;
			printf("Pair Success button press \r\n");
			PostFarmerMasterSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'L'){
			ReturnEvent.EventType = ES_LOST_CONNECTION;
			printf("Lost connection button press\r\n");
			PostFarmerMasterSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'R'){
			ReturnEvent.EventType = ES_RESEND_ENCRYPT;
			printf("Resend Encrypt button press\r\n");
			PostFarmerMasterSM(ReturnEvent);
		}else if(ThisEvent.EventParam == 'F'){
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			printf("FarmerTX-----SENDING MESSAGE EVENT--------\r\n");
			PostFarmerTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == '1'){
			setFarmerDataHeader(REQ_2_PAIR);
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostFarmerTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == '2'){
			setFarmerDataHeader(ENCR_KEY);
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostFarmerTXSM(ReturnEvent);
		}else if(ThisEvent.EventParam == '3'){
			setFarmerDataHeader(CTRL);
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			PostFarmerTXSM(ReturnEvent);
		}else{   // otherwise post to Service 0 for processing
    }
    return true;
  }
  return false;
}

bool CheckSound(void)
{
	bool ReturnVal = false;
	// initialize Volume level
	static uint16_t LastVolume = 0;
	// sample volume
	uint32_t Volume[1];
	ADC_MultiRead(Volume);
	// if the volume is crossing over the threshold
	if ((Volume[0] >= VOLUME_THRESHOLD) && (LastVolume < VOLUME_THRESHOLD))
	{
		ES_Event ReturnEvent;
		ReturnEvent.EventType = ES_SPEECH_DETECTED;
		PostFarmerMasterSM(ReturnEvent);
		printf("Audio input detected\r\n"); ///POSTS A BUNCH OF TIMES
		ReturnVal = true;
	}
	//printf("Volume: %i\r\n",Volume[0]);
	LastVolume = Volume[0];
	return ReturnVal;
}

bool CheckButton(void)
{
	bool ReturnVal = false;
	// initialize last button state
	static bool R_Button_Last = 0;
	static bool L_Button_Last = 0;
	static bool REV_Button_Last = 0;
	static bool P_Button_Last = 0;
	
	// get current state of button
	bool R_Button = ((HWREG(GPIO_PORTB_BASE + (ALL_BITS + GPIO_O_DATA)) & R_BUTTON_B) == R_BUTTON_B);
	bool L_Button = ((HWREG(GPIO_PORTB_BASE + (ALL_BITS + GPIO_O_DATA)) & L_BUTTON_B) == L_BUTTON_B);
	bool REV_Button = ((HWREG(GPIO_PORTD_BASE + (ALL_BITS + GPIO_O_DATA)) & REVERSE_BUTTON_D) == REVERSE_BUTTON_D);
	bool P_Button = ((HWREG(GPIO_PORTD_BASE + (ALL_BITS + GPIO_O_DATA)) & PERIPHERAL_BUTTON_D) == PERIPHERAL_BUTTON_D);
	
	// if the current button state is not the last button state, post the appropriate event
	if (R_Button != R_Button_Last)
	{
		ES_Event Event2Post_R;
		if (R_Button)
		{
			Event2Post_R.EventType = ES_BUTTON_UP;
		}
		else
		{
			Event2Post_R.EventType = ES_BUTTON_DOWN;
		}
		PostR_ButtonDebounce(Event2Post_R);
		ReturnVal = true;
	}
	
	if (L_Button != L_Button_Last)
	{
		ES_Event Event2Post_L;
		if (L_Button)
		{
			Event2Post_L.EventType = ES_BUTTON_UP;
		}
		else
		{
			Event2Post_L.EventType = ES_BUTTON_DOWN;
		}
		PostL_ButtonDebounce(Event2Post_L);
		ReturnVal = true;
	}
	
	if (REV_Button != REV_Button_Last)
	{
		ES_Event Event2Post_REV;
		if (REV_Button)
		{
			Event2Post_REV.EventType = ES_BUTTON_UP;
		}
		else
		{
			Event2Post_REV.EventType = ES_BUTTON_DOWN;
		}
		PostREV_ButtonDebounce(Event2Post_REV);
		ReturnVal = true;
	}
	
	if (P_Button != P_Button_Last)
	{
		ES_Event Event2Post_P;
		if (P_Button)
		{
			Event2Post_P.EventType = ES_BUTTON_UP;
		}
		else
		{
			Event2Post_P.EventType = ES_BUTTON_DOWN;
		}
		PostP_ButtonDebounce(Event2Post_P);
		ReturnVal = true;
	}
	
	L_Button_Last = L_Button;
	R_Button_Last = R_Button;
	REV_Button_Last = REV_Button;
	P_Button_Last = P_Button;
	
	return ReturnVal;
}
