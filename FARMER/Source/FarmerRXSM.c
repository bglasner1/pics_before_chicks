/****************************************************************************
 Module
   Farmer_RX_SM.c

 Revision
   1.0.1

 Description
   The receiving state machine for the Farmer

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 05/13/17 5:29	mwm			created for the project
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "FarmerRXSM.h"
#include "Constants.h"
#include "FarmerTXSM.h"

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


/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void DataInterpreter( void );
static void ClearDataArray( void );
static void InterpretPairAck(void);
static void InterpretEncrReset(void);
static void InterpretStatus(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static FarmerRX_State_t CurrentState;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority, memCnt; //,paired
static uint8_t DogAddrMSB;
static uint8_t DogAddrLSB;
static bool paired;
static uint16_t BytesLeft,DataLength,TotalBytes;
static uint8_t Data[RX_DATA_LENGTH] = {0};


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitFarmerRXSM

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
bool InitFarmerRXSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;
  // put us into the first state
  CurrentState = WaitForFirstByte;
  // post the initial transition event
	//Set memCnt to 0
	memCnt = 0;
	//Start ConnectionTimer for 1 second
	ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
	//Set paired to false
	paired = false;
	
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
     PostFarmerRXSM

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
bool PostFarmerRXSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunFarmerRXSM

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
ES_Event RunFarmerRXSM( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch ( CurrentState )
  {
		//Case WaitForFirstByte
		case WaitForFirstByte:
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER){
				//if device paired
				if(paired){
					//Post ES_LOST_CONNECTION to Farmer_Master_SM
				}
				//Set memCnt to 0
				memCnt = 0;
				
				//Start ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}else if(ThisEvent.EventType == ES_BYTE_RECEIVED && Data[0] == INIT_BYTE){
			//if ThisEvent EventType is ES_BYTE_RECEIVED and EventParam byte is 0x7E
				//Set CurrentState to WaitForMSBLen
				CurrentState = WaitForMSBLen;
				
				//Increment memCnt
				memCnt++;
				
				//Restart ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			break;

		//Case WaitForMSBLen
		case WaitForMSBLen :
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER){
				//Set CurrentState to WaitForFirstByte
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				//Clear Data array
				ClearDataArray();
				
				//Post ES_LOST_CONNECTION to Farmer_Master_SM
				
				//Start ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			//if ThisEvent EventType is ES_BYTE_RECEIVED
			if(ThisEvent.EventType == ES_BYTE_RECEIVED){
				//Set CurrentState to WaitForLSBLen
				CurrentState = WaitForLSBLen;
				
				//Increment memCnt
				memCnt++;
				
				//Restart ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			break;
		
		//Case WaitForLSBLen
		case WaitForLSBLen :
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER){
				//Set CurrentState to WaitForFirstByte
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				//Clear Data array
				ClearDataArray();
				
				//Post ES_LOST_CONNECTION to Farmer_Master_SM
				
				//Start ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			//if ThisEvent EventType is ES_BYTE_RECEIVED
			if(ThisEvent.EventType == ES_BYTE_RECEIVED){
				//Set CurrentState to AcquireData
				CurrentState = AcquireData;
				
				//Increment memCnt
				memCnt++;
				
				//Combine Data[1] and Data[2] into BytesLeft and DataLength
				BytesLeft = Data[1];
				BytesLeft = (BytesLeft << 8) + Data[2];
				DataLength = BytesLeft;
				TotalBytes = DataLength+NUM_XBEE_BYTES;
				
				//Restart ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			break;

		//Case AcquireData
		case AcquireData :
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER){
				//Set CurrentState to WaitForFirstByte
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				//Clear Data array
				ClearDataArray();
				
				//Post ES_LOST_CONNECTION to Farmer_Master_SM
				
				//Start ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}else if(ThisEvent.EventType == ES_BYTE_RECEIVED && BytesLeft !=0){
			//if ThisEvent EventType is ES_BYTE_RECEIVED and BytesLeft != 0
				//Increment memCnt
				memCnt++;
				
				//Restart ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
				
				//Decrement BytesLeft
				BytesLeft--;
			}else if(ThisEvent.EventType == ES_BYTE_RECEIVED && BytesLeft == 0){
			//if ThisEvent EventType is ES_BYTE_RECEIVED and BytesLeft == 0
				//Set CurrentState to WaitForFirstByte
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				//Restart ConnectionTimer for 1 second
				ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
				
				//Run DataInterpreter
				DataInterpreter();
				
				//Clear Data Array
				ClearDataArray();
			}
			break;
    default :
      ;
  }  // end switch on Current State
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryFarmerRXSM

 Parameters
     None

 Returns
     FarmerRX_State_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
Matthew Miller, 5/13/17, 22:42
****************************************************************************/
FarmerRX_State_t QueryFarmerRXSM ( void )
{
   return(CurrentState);
}
/****************************************************************************
 Function
     FarmerRX_ISR

 Parameters
     None

 Returns
     The interrupt response for the UART receive

 Description
     stores the received byte into the data
 Notes

 Author
Matthew Miller, 5/13/17, 22:42
****************************************************************************/
void FarmerRX_ISR( void ){
	//printf(".");
	ES_Event ReturnEvent;
	//Set data to the current value on the data register
	Data[memCnt] = HWREG(UART1_BASE + UART_O_DR);
	ReturnEvent.EventType = ES_BYTE_RECEIVED;
	PostFarmerRXSM(ReturnEvent);
	
	//Check and handle receive errors
	if((HWREG(UART1_BASE + UART_O_RSR) & UART_RSR_OE) != 0){
		printf("Overrun Error :(\r\n");
	}
	if((HWREG(UART1_BASE + UART_O_RSR) & UART_RSR_BE) != 0){
		printf("Break Error :(\r\n");
	}
	if((HWREG(UART1_BASE + UART_O_RSR) & UART_RSR_FE) != 0){
		printf("Framing Error :(\r\n");
	}
	if((HWREG(UART1_BASE + UART_O_RSR) & UART_RSR_PE) != 0){
		printf("Parity Error :(\r\n");
	}
	HWREG(UART1_BASE + UART_O_ECR) |= UART_ECR_DATA_M;
}

void RXTX_ISR( void ){
	//get status of the receive and transmit interrupts
	uint8_t RX_Int = HWREG(UART1_BASE + UART_O_MIS) & UART_MIS_RXMIS;
	uint8_t TX_Int = HWREG(UART1_BASE + UART_O_MIS) & UART_MIS_TXMIS;
	
	//If there was a receive interrupt
	if(RX_Int != 0){
		//Clear the source of the interrupt
		HWREG(UART1_BASE + UART_O_ICR) |= UART_ICR_RXIC;
		//Call the farmer receive interrupt response
		FarmerRX_ISR();
	}
	
	//If there was a transmit interrupt
	if(TX_Int != 0){
		//Clear the source of the interrupt
		HWREG(UART1_BASE + UART_O_ICR) |= UART_ICR_TXIC;
		//Call the farmer transmit interrupt response
		FarmerTX_ISR();
	}
}

/***************************************************************************
 private functions
 ***************************************************************************/
static void DataInterpreter()
{
	for(int i = 0; i<TotalBytes;i++)
	{
		printf("Bit %i: %04x\r\n",i,Data[i]);
	}
	
	//********IF PAIRED IGNORE MESSAGE IF IT IS NOT THE DOG YOU ARE PAIRED WITH****************//
	//********MIGHT WANT TO PUT THIS FUNCTIONALITY DURING RECEIVE OF MESSAGE SO IT DOESN'T LISTEN TO THE WHOLE THING************//
	//If currently paired 
		//Check to see which DOG you are paired with
		//If the DOG that sent the message is not the DOG you are paired with
			//Clear the data array
			//Return
		//EndIf
	//EndIf
	
	//If DataHeader is PAIR_ACK
		//Call Interpret PAIR_ACK message
	//Else If DataHeader is ENCR_RESET
		//Call Interpret ENCR_RESET message
	//Else If DataHeader is STATUS
		//Call Interpret STATUS message
	//EndIf
	
	//Clear data array
}

static void InterpretPairAck(void)
{
	//Set DogAddrMSB to Sender address MSB
	//Set DogAddrLSB to Sender address LSB
	//Set destination address in FarmerTXSM to DogAddrMSB and DogAddrLSB
	//Set paired to true
	//Post ES_PAIR_SUCCESSFUL to Farmer_Master_SM
}

static void InterpretEncrReset(void)
{
	//Set DataHeader to ENCR_KEY in FarmerTXSM
}

static void InterpretStatus(void)
{
	//local variable AttitudeIndex
	//Initialize AttitudeIndex to RX_PREAMBLE_LENGTH + 1 (start after the header)
	
	//Set the AccelX bytes in the Attitude module to the AccelXData bytes from Data array
	//Set the AccelY bytes in the Attitude module to the AccelYData bytes from Data array
	//Set the AccelZ bytes in the Attitude module to the AccelZData bytes from Data array
	
	//Set the GyroX bytes in the Attitude module to the GyroXData bytes from Data array
	//Set the GyroY bytes in the Attitude module to the GyroYData bytes from Data array
	//Set the GyroZ bytes in the Attitude module to the GyroZData bytes from Data array
}


static void ClearDataArray( void ){
	for(int i = 0; i<RX_DATA_LENGTH;i++){
		Data[i] = 0;
	}
}
