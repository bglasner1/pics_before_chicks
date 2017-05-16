/****************************************************************************
 Module
   Dog_TX_SM.c

 Revision
   1.0.1

 Description
   The receiving state machine for the Dog

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
#include "DogTXSM.h"
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

/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
static void MessageTransmitted( void );
static void ClearMessageArray( void );

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static DogTX_State_t CurrentState;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority, TransEnable, MessIndex, BytesRemaining;
static uint8_t Message[TX_MESSAGE_LENGTH] = {0};


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDogTXSM

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
bool InitDogTXSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;
  // put us into the first state
  CurrentState = Waiting2Transmit;

	//Start TransmitTimer for 200 ms
	ES_Timer_InitTimer(TRANS_TIMER, TRANSMISSION_RATE);
	//Set Trans_Enable to false
	TransEnable = false;
	Message[0] = INIT_BYTE;
	Message[1] = 0x00;
	Message[2] = 0x0A;
	Message[3] = 0x01;
	Message[4] = 0x01;
	Message[5] = 0x20;
	Message[6] = 0x81;
	Message[7] = 0x00;
	Message[8] = 0x10;
	Message[9] = 0x11;
	Message[10] = 0x12;
	Message[11] = 0x13;
	Message[12] = 0x14;
	uint8_t sum = 0;
	for(int i = 3; i<13;i++){
		sum += Message[i];
	}
	//printf("Sum: %i\r\n",sum);
	Message[13] = 0xFF-sum;
	
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
     PostDogTXSM

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
bool PostDogTXSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDogTXSM

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
ES_Event RunDogTXSM( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch ( CurrentState )
  {
		//Case Waiting2Transmit
		case Waiting2Transmit :	
			//If ThisEvent is ES_TIMEOUT and Transmit is enabled
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == TRANS_TIMER && TransEnable){
				//Set CurrentState to Transmit
				CurrentState = Transmit;
				//Build the message to send
				//Reset the message counter (packet byte index)
				MessIndex = 0;
				BytesRemaining = TX_MESSAGE_LENGTH;
				//if TXFE clear
				if((HWREG(UART1_BASE+UART_O_FR) & UART_FR_TXFE) != 0){
					//Write first byte of the message to send into the UART data register
					HWREG(UART1_BASE+UART_O_DR) = Message[MessIndex];
					//decrement BytesRemaining
					BytesRemaining--;
					//increment messIndex
					MessIndex++;
					//if TXFe clear
					if((HWREG(UART1_BASE+UART_O_FR) & UART_FR_TXFE) != 0){
						//Write second byte of the message to send into the UART data register
						HWREG(UART1_BASE+UART_O_DR) = Message[MessIndex];
						//decrement BytesRemaining
						BytesRemaining--;
						//increment messIndex
						MessIndex++;
					}
					//Enable Tx interrupts in the UART
					HWREG(UART1_BASE + UART_O_IM) = HWREG(UART1_BASE + UART_O_IM) | UART_IM_TXIM;
				}
			}else{
				//Restart TRANS_TIMER for TRANSMISSION_RATE
				ES_Timer_InitTimer(TRANS_TIMER, TRANSMISSION_RATE);
			}
			
			break;

		//Case Transmit
		case Transmit :
			//If ThisEvent is ES_TRANSMIT_COMPLETE
			if(ThisEvent.EventType == ES_TRANSMIT_COMPLETE){
				
				//Set CurrentState to Waiting2Transmit
				CurrentState = Waiting2Transmit;
				//Restart TRANS_TIMER for TRANSMISSION_RATE
				ES_Timer_InitTimer(TRANS_TIMER, TRANSMISSION_RATE);
				
				//Set TransEnable to false
				TransEnable = false;
				MessageTransmitted();
			}
			break;
    default :
      ;
  }  // end switch on Current State
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryDogTXSM

 Parameters
     None

 Returns
     DogTX_State_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
Matthew Miller, 5/13/17, 22:42
****************************************************************************/
DogTX_State_t QueryDogTXSM ( void )
{
   return(CurrentState);
}
/****************************************************************************
 Function
     DogTX_ISR

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
void DogTX_ISR( void ){
	//Write next byte of message 
	HWREG(UART1_BASE+UART_O_DR) = Message[MessIndex];
	
	//Decrement BytesRemaining
	BytesRemaining--;
	
	//Increment messIndex
	MessIndex++;
	
	//If BytesRemaining is 0
	if(BytesRemaining == 0){
		//Disable interrupt on TX
		HWREG(UART1_BASE + UART_O_IM) = HWREG(UART1_BASE + UART_O_IM) & ~UART_IM_TXIM;
		
		//Post ES_TRANSMIT_COMPLETE event
		ES_Event ReturnEvent;
		ReturnEvent.EventType = ES_TRANSMIT_COMPLETE;
		PostDogTXSM(ReturnEvent);
	}
}

void enableTransmit( void ){
	TransEnable = true;
	return;
}

/***************************************************************************
 private functions
 ***************************************************************************/
static void MessageTransmitted(){
	for(int i = 0; i<TX_MESSAGE_LENGTH;i++){
		printf("Message %i: %04x\r\n",i,Message[i]);
	}
	return;
}

static void ClearMessageArray( void ){
	for(int i = 0; i<TX_MESSAGE_LENGTH;i++){
		Message[i] = 0;
	}
	return;
}

void sendToPIC(uint8_t value){
	printf("Sent To PIC: %i\r\n",value);
	if((HWREG(UART3_BASE+UART_O_FR) & UART_FR_TXFE) != 0){
		HWREG(UART3_BASE+UART_O_DR) = value;
	}
}

