#include "ES_Configure.h"
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
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"

#include "Hardware.h"
#include "Constants.h"
#include "I2C_Service.h"

static uint8_t MyPriority;
static I2C_State CurrentState = I2C_Init;

static uint16_t Read_I2C(uint8_t IMU_Reg, uint8_t Num_Regs);

bool Init_I2C(uint8_t Priority)
{
	// set local priority
  MyPriority = Priority;
  // set timer to allow I2C to hookup
	ES_Timer_InitTimer(IMU_TIMER, I2C_DELAY_TIME);
	// state is init
	
	return true;
}

bool Post_I2C(ES_Event ThisEvent)
{
  return ES_PostToService( MyPriority, ThisEvent);
}

ES_Event Run_I2C( ES_Event ThisEvent )
{
	I2C_State NextState = CurrentState;
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	// loop through states
	switch (CurrentState)
	{
		case (I2C_Init):
		{
			// if event is IMU_Timeout
			if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == IMU_TIMER))
			{
				// Write first byte to output
				uint16_t WHO_AM_I = Read_I2C((uint8_t)0x0F, 1);
				printf("\r\nWHO AM I: %d\r\n", WHO_AM_I);
			}
			break;
		}
	}
	
  return ReturnEvent;
}


//Function to read a byte from one of the slave's register addresses via I2C
static uint16_t Read_I2C(uint8_t IMU_Reg, uint8_t Num_Regs) 
{
	uint16_t ReturnVal = 0;
  // set transmission mode
  HWREG(I2C2_BASE + I2C_O_MSA) &= ~I2C_MSA_RS;
  // load the desired register value to be sent
  HWREG(I2C2_BASE + I2C_O_MDR) = IMU_Reg;
	// set a multiple byte transmission
  HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
  // This is blocking, very depressing
  while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
  // insert blocking delay, this is really messed up
  for(int i = 0; i < 500; i += 1) {}
  // set receive mode
  HWREG(I2C2_BASE + I2C_O_MSA) |= I2C_MSA_RS;
  // set a multiple byte receive
  HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_RX;
  // Blocking code is just the pits
  while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
	if (Num_Regs == 2)
	{
		// set a multiple byte receive
		HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_RX;
		// Blocking code is just the pits
		while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
		// Read Data
		ReturnVal |= ((HWREG(I2C2_BASE + I2C_O_MDR) & 0xff) << 8);
	}
  // set a single byte receive
  HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_SINGLE_RX;
  // beating a dead horse something something Chris Brown
  while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
  //Read the data
	ReturnVal |= (HWREG(I2C2_BASE + I2C_O_MDR) & 0xff);
  return ReturnVal;
}