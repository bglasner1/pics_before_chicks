/****************************************************************************
 Module
   Dog_RX_SM.c

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
#include "DogRXSM.h"
#include "Constants.h"
#include "DogTXSM.h"
#include "DogMasterSM.h"

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
static void HandleEncr( void );
static void HandleCtrl( void );
static void HandleReq( void );
static void DecryptData( void );
static void ResetEncr( void );
static void setPair( void );
static void LostConnection( void );

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static DogRX_State_t CurrentState;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority, memCnt, paired, TurnData, MoveData, PerData, BrakeData, Broadcast;
static uint8_t MSB_Address, LSB_Address, EncryptCnt, RecDogTag;
static uint16_t BytesLeft,DataLength,TotalBytes;
static uint8_t Data[RX_MESSAGE_LENGTH] = {0};
static uint8_t Encryption[ENCR_LENGTH] = {0};


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDogRXSM

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
bool InitDogRXSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;
  // put us into the first state
  CurrentState = WaitForFirstByte;
  // post the initial transition event
	//Set memCnt to 0
	memCnt = 0;
	//Start ConnectionTimer for 1 second
	//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
	//Set paired to false
	paired = false;
	
	// connect clock to ports B
	HWREG(SYSCTL_RCGCGPIO) |= (SYSCTL_RCGCGPIO_R1);
	// wait for clock to connect to ports B and F
	while ((HWREG(SYSCTL_PRGPIO) & (SYSCTL_PRGPIO_R1)) != (SYSCTL_PRGPIO_R1)) {}
	// digitally enable IO pins
	HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) |= (GPIO_PIN_1);
	// set direction of IO pins
	HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) |= (GPIO_PIN_1);
	
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
     PostDogRXSM

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
bool PostDogRXSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDogRXSM

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
ES_Event RunDogRXSM( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	//printf("DogRXCurrentState = %i\r\n",CurrentState);
  switch ( CurrentState )
  {
		//Case WaitForFirstByte
		case WaitForFirstByte:
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER)
			{
				//if device paired
				if(paired)
				{
					LostConnection();
				
				}
				//Set memCnt to 0
				memCnt = 0;
				
				//Start ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			else if(ThisEvent.EventType == ES_BYTE_RECEIVED && Data[0] == INIT_BYTE)
			{
			//if ThisEvent EventType is ES_BYTE_RECEIVED and EventParam byte is 0x7E
				//Set CurrentState to WaitForMSBLen
				CurrentState = WaitForMSBLen;
				
				//Increment memCnt
				//memCnt++;
				
				//Restart ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			break;

		//Case WaitForMSBLen
		case WaitForMSBLen :
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER)
			{
				//Set CurrentState to WaitForFirstByte
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				LostConnection();
				
				//Start ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			//if ThisEvent EventType is ES_BYTE_RECEIVED
			if(ThisEvent.EventType == ES_BYTE_RECEIVED){
				//Set CurrentState to WaitForLSBLen
				CurrentState = WaitForLSBLen;
				
				//Increment memCnt
				//memCnt++;
				
				//Restart ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
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
				
				LostConnection();
				
				//Start ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			//if ThisEvent EventType is ES_BYTE_RECEIVED
			if(ThisEvent.EventType == ES_BYTE_RECEIVED){
				//Set CurrentState to AcquireData
				CurrentState = AcquireData;
				
				//Increment memCnt
				//memCnt++;
				
				//Combine Data[1] and Data[2] into BytesLeft and DataLength
				//BytesLeft = Data[1];
				BytesLeft = Data[2];
				//printf("Data[2] = %x\r\n",Data[2]);
				DataLength = BytesLeft;
				TotalBytes = DataLength+NUM_XBEE_BYTES;
				
				//Restart ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}
			break;

		//Case AcquireData
		case AcquireData :
			//if ThisEvent EventType is ES_Timeout and EventParam is ConnectionTimer
			if(ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam == CONN_TIMER){
				//Set CurrentState to WaitForFirstByte
				//printf("Dog RX SM -- Acquire Data State -- Timer Timeout\r\n");
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				LostConnection();
				
				//Start ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
			}else if(ThisEvent.EventType == ES_BYTE_RECEIVED && BytesLeft !=0){
			//if ThisEvent EventType is ES_BYTE_RECEIVED and BytesLeft != 0
				//Increment memCnt
				//memCnt++;
				
				//Restart ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
				
				//Decrement BytesLeft
				BytesLeft--;
				//printf("DogRXSM -- AcquireData State -- %x\r\n",BytesLeft);
			}else if(ThisEvent.EventType == ES_BYTE_RECEIVED && BytesLeft == 0){
			//if ThisEvent EventType is ES_BYTE_RECEIVED and BytesLeft == 0
				//Set CurrentState to WaitForFirstByte
				CurrentState = WaitForFirstByte;
				
				//Set memCnt to 0
				memCnt = 0;
				
				//Restart ConnectionTimer for 1 second
				//ES_Timer_InitTimer(CONN_TIMER, CONNECTION_TIME);
				printf("Dog RX SM -- Acquire Data State -- Message Received\r\n");
				//Run DataInterpreter
				DataInterpreter();
				
				//Clear Data Array
				ClearDataArray();
			}else{
			printf("Dog RX SM -- Acquire Data State -- Other\r\n");
			}
			break;
    default :
      ;
  }  // end switch on Current State
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryDogRXSM

 Parameters
     None

 Returns
     DogRX_State_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
Matthew Miller, 5/13/17, 22:42
****************************************************************************/
DogRX_State_t QueryDogRXSM ( void )
{
   return(CurrentState);
}
/****************************************************************************
 Function
     DogRX_ISR

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
void DogRX_ISR( void ){
	//printf(".\r\n");
	ES_Event ReturnEvent;
	//Set data to the current value on the data register
	Data[memCnt] = HWREG(UART1_BASE + UART_O_DR);
	//printf(".");
	//printf("%x\r\n", memCnt);
	memCnt++;
	ReturnEvent.EventType = ES_BYTE_RECEIVED;
	PostDogRXSM(ReturnEvent);
	
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
		//Call the Dog receive interrupt response
		DogRX_ISR();
	}
	
	//If there was a transmit interrupt
	if(TX_Int != 0){
		//Clear the source of the interrupt
		HWREG(UART1_BASE + UART_O_ICR) |= UART_ICR_TXIC;
		//Call the Dog transmit interrupt response
		DogTX_ISR();
	}
}

/***************************************************************************
 private functions
 ***************************************************************************/
static void DataInterpreter(){
	
	if(paired)
	{
		//Check to see which FARMER you are paired with
		//If the DOG that sent the message is not the FARMER you are paired with
		printf("Data Interpretation -- Farmer ID Mismatch -- IGNORE MESSAGE\r\n");
		if((Data[4] != MSB_Address) || (Data[5] != LSB_Address))
		{
			//Clear the data array
			ClearDataArray();
			//Return
			return;
		}
		//EndIf
	}//EndIf
	printf("Dog RX SM -- Data Interpreter -- Top\r\n");
	for(int i = 0; i<TotalBytes;i++){
		printf("Byte %i: %04x\r\n",i,Data[i]);
	}
	// if Data[3] equals API_81
	if(Data[3] == API_81){
		DecryptData();
			for(int i = 0; i<TotalBytes;i++){
				printf("Decrypted Byte %i: %04x\r\n",i,Data[i]);
			}
	
		//if Data[8] equals REQ_2_PAIR
		if(Data[8] == REQ_2_PAIR){
			//Call HandleReq()
			HandleReq();
		//elseif Data[8] equals ENCR_KEY
		}else if(Data[8] == ENCR_KEY){
			//Call HandleEncr()
			HandleEncr();
		//elseif Data[8] equals CTRL
		}else if(Data[8] == CTRL){
			//Call HandleCtrl()
			HandleCtrl();
		}else{
			printf("Dog RX SM -- Data Interpreter -- Invalid Header\r\n");
			//Call ResetEncr
			ResetEncr();
			//Call setDogDataHeader with ENCR_RESET parameter
			setDogDataHeader(ENCR_RESET);
			//Post transmit ENCR_RESET Event to TX_SM
			ES_Event ReturnEvent;
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			//PostDogTXSM(ReturnEvent);
		}
	}
}

static void ClearDataArray( void ){
	for(int i = 0; i<RX_DATA_LENGTH;i++){
		Data[i] = 0;
	}
}

static void HandleEncr( void ){
	printf("Dog RX SM -- Handle Encryption -- Top\r\n");
	// if paired
	if(paired){
		printf("Dog RX SM -- Handle Encryption -- If Paired\r\n");
		ResetEncr();
		// Call setDogDataHeader with ENCR_RESET parameter
		setDogDataHeader(ENCR_RESET);
		//Post transmit ENCR_RESET Event to TX_SM
		ES_Event ReturnEvent;
		ReturnEvent.EventType= ES_SEND_RESPONSE;
		//PostDogTXSM(ReturnEvent);
	}else{
		printf("Dog RX SM -- Handle Encryption -- If Unpaired\r\n");
		//for each of the elements of the encryption array set it equal to the corresponding data location
		for(int i = 0; i < ENCR_LENGTH; i++){
			Encryption[i] = Data[i+9];
		}
		EncryptCnt = 0;
		setPair();
		ES_Event ReturnEvent;
		ReturnEvent.EventType = ES_PAIR_SUCCESSFUL;
		PostDogMasterSM(ReturnEvent);
		printf("Set the Encryption Key\r\n");
		
		// Send status
		setDogDataHeader(STATUS);
		//Post transmit STATUS Event to TX_SM
		ReturnEvent.EventType = ES_SEND_RESPONSE;
		//PostDogTXSM(ReturnEvent);
		printf("Sent Status\r\n");
	}
}

static void HandleCtrl( void ){
	printf("Dog RX SM -- Handle Control -- Top\r\n");
	// if paired
	if(paired){
		printf("Dog RX SM -- Handle Control -- Paired\r\n");
		//Call setDogDataHeader with STATUS parameter
		setDogDataHeader(STATUS);
		//Post transmit STATUS Event to TX_SM
		ES_Event ReturnEvent;
		ReturnEvent.EventType = ES_SEND_RESPONSE;
		//PostDogTXSM(ReturnEvent);
		//if MoveData is greater than 127
		if(MoveData > DATA_MIDPOINT){
			// Set forward fan to digital or analog value
			printf("Move forward fan\r\n");
		//elseif MoveData is less than 127
		}else if(MoveData < DATA_MIDPOINT){
			// Set reverse fan to digital or analog value
			printf("Move reverse fan\r\n");
		}
		//if TurnData is greater than 127
		if(TurnData > DATA_MIDPOINT){
			// Turn right servo on
			printf("Turn Right Servo\r\n");
		//elseif TurnData is less than 127
		}else if(TurnData < DATA_MIDPOINT){
			// Turn left servo on
			printf("Turn Left Servo\r\n");
		}
		//if PerData is greater than 0
		if(PerData > 0){
			// Toggle peripheral functionality (lift fan maybe)
			printf("Peripheral functionality Engaged\r\n");
		}
		//if BrakeData is greater than 0
		if(BrakeData > 0){
			// Turn both servos on (lift fan maybe)
			printf("Brake functionality Engaged\r\n");
		} else {
			// Turn both servos off (lift fan maybe)
			printf("Brake functionality Disengaged\r\n");
		}
	} else {
		printf("Dog RX SM -- Handle Control -- Unpaired\r\n");
			//Call ResetEncr
			ResetEncr();
			//Call setDogDataHeader with ENCR_RESET parameter
			setDogDataHeader(ENCR_RESET);
			//Post transmit ENCR_RESET Event to TX_SM
			ES_Event ReturnEvent;
			ReturnEvent.EventType = ES_SEND_RESPONSE;
			//PostDogTXSM(ReturnEvent);
	}
}

static void HandleReq( void ){
	printf("Dog RX SM -- Handle Request -- Top\r\n");
	// if paired and not a broadcast
	if(paired && Broadcast == 0){
		printf("Dog RX SM -- Handle Request -- Improper Time\r\n");
		//Call ResetEncr
		ResetEncr();
		
		//Call setDogDataHeader with ENCR_RESET parameter
		setDogDataHeader(ENCR_RESET);
		
		//Post transmit ENCR_RESET Event to TX_SM
		ES_Event ReturnEvent;
		ReturnEvent.EventType = ES_SEND_RESPONSE;
		//PostDogTXSM(ReturnEvent);
		printf("Paired request while paired and it isn't a broadcast\r\n");
	}else if(!paired && ((Broadcast&BROAD_MASK) != 0) && (RecDogTag == getDogTag())){
		printf("Dog RX SM -- Handle Request -- Proper Time\r\n");
		//Call setDogDataHeader with PAIR_ACK parameter
		setDogDataHeader(PAIR_ACK);
		
		//Set Destination address of Farmer
		setDestFarmerAddress(MSB_Address,LSB_Address);
		
		//Post transmit ENCR_RESET Event to TX_SM
		ES_Event ReturnEvent;
		ReturnEvent.EventType = ES_SEND_RESPONSE;
		//PostDogTXSM(ReturnEvent);
		ReturnEvent.EventType = ES_BROADCAST_RECEIVED;
		PostDogMasterSM(ReturnEvent);
	}
}

static void DecryptData( void ){
	printf("Dog RX SM -- Decrypt Data -- Top\r\n");
	//for each of the elements of the dataBuffer
	for(int i = 0; i < MAX_DATA_LENGTH; i++){
		// set data equal to dataBuffor xor with Encryption Key
		Data[i+RX_DATA_OFFSET] = Data[i+RX_DATA_OFFSET]^Encryption[EncryptCnt];
		EncryptCnt++;
		EncryptCnt = EncryptCnt%32;
	}
	//Set MSB_Address
	MSB_Address = Data[4];
	printf("Dog RX SM -- Decrypt Data -- MSB_Address = %04x\r\n", MSB_Address);
	
	//Set LSB_Address
	LSB_Address = Data[5];
	printf("Dog RX SM -- Decrypt Data -- LSB_Address = %04x\r\n", LSB_Address);
	
	//Set TurnData
	TurnData = Data[10];
	printf("Dog RX SM -- Decrypt Data -- TurnData = %i\r\n", TurnData);
	
	//Set MoveData
	MoveData = Data[9];
	printf("Dog RX SM -- Decrypt Data -- MoveData = %i\r\n", MoveData);
	
	//Set Brake
	BrakeData = Data[11] & BRAKE_MASK;
	printf("Dog RX SM -- Decrypt Data -- BrakeData = %i\r\n", BrakeData);
	
	//Set Peripheral
	PerData = Data[11] & PER_MASK;
	printf("Dog RX SM -- Decrypt Data -- Peripheral = %i\r\n", PerData);
	
	//Set Broadcasted
	Broadcast = Data[7];
	printf("Dog RX SM -- Decrypt Data -- Broadcast = %i\r\n", Broadcast);
	
	//Set Received DogTag
	RecDogTag = Data[9];
	printf("Dog RX SM -- Decrypt Data -- RecDogTag = %i\r\n", RecDogTag);
}

static void ResetEncr( void ){
	//for each of the elements of the encryption array set it equal to 0x00000000
	for(int i = 0; i<ENCR_LENGTH; i++){
		Encryption[i] = 0x00;
	}
	EncryptCnt = 0;
}

static void setPair( void ){
	//Set paired to 1
	paired = true;
}
static void clearPair( void ){
	//Set paired to 0
	paired = false;
}
static void LostConnection( void ){
	ClearDataArray();
	ResetEncr();
	clearPair();
	//Post ES_LOST_CONNECTION to Dog_Master_SM
	ES_Event NewEvent;
	NewEvent.EventType = ES_LOST_CONNECTION;
	PostDogMasterSM(NewEvent);
}