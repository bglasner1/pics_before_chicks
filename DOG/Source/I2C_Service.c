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
static uint16_t Accel_X = 0;
static uint16_t Accel_Y = 0;
static uint16_t Accel_Z = 0;
static uint16_t Gyro_X = 0;
static uint16_t Gyro_Y = 0;
static uint16_t Gyro_Z = 0;
static uint16_t Accel_X_OFF = 0;
static uint16_t Accel_Y_OFF = 0;
static uint16_t Accel_Z_OFF = 0;
static uint16_t Gyro_X_OFF = 0;
static uint16_t Gyro_Y_OFF = 0;
static uint16_t Gyro_Z_OFF = 0;

static uint16_t Read_I2C(uint8_t IMU_Reg);
static void Send_I2C(uint8_t IMU_Reg, uint8_t IMU_Data);
static void Offset_Init(void);

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
		// if state is init
		case (I2C_Init):
		{
			// if event is IMU_Timeout
			if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == IMU_TIMER))
			{
				// initialize Gyro/accelerometer power settings
				Send_I2C(GYROSCOPE_POWER_REGISTER, GYROSCOPE_POWER_SETTING);
				Send_I2C(ACCELEROMETER_POWER_REGISTER, ACCELEROMETER_POWER_SETTING);
				
				// Testing
				uint16_t WHO_AM_I = Read_I2C((uint8_t)0x0F);
				printf("\r\nWHO AM I: %d\r\n", WHO_AM_I);
				uint16_t Echo = Read_I2C(ACCELEROMETER_POWER_REGISTER);
				printf("\r\nAccel power reg: %d\r\n", Echo);
				Echo = Read_I2C(GYROSCOPE_POWER_REGISTER);
				printf("\r\nGyro power reg: %d\r\n", Echo);
				
				printf("\r\nGyro X\tGyro Y\tGyro Z\tAccel X\tAccel Y\tAccel Z\r\n");
				
				// Get offsets
				Offset_Init();
				// set IMU Timer
				ES_Timer_InitTimer(IMU_TIMER, IMU_POLL_TIME);
				// next state is poll
				NextState = I2C_Poll_IMU;
			}
			break;
		}
		// else if state is poll
		case (I2C_Poll_IMU):
		{
			// if event is timeout
			if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == IMU_TIMER))
			{
				// reset timer
				ES_Timer_InitTimer(IMU_TIMER, IMU_POLL_TIME);
				// Read all data
				Gyro_X = Read_I2C(GYROSCOPE_X_REGISTER_BASE);
				Gyro_X |= (Read_I2C(GYROSCOPE_X_REGISTER_BASE + 1) << 8);
				Gyro_X = Gyro_X - Gyro_X_OFF;
				printf("%d\t", Gyro_X);
				Gyro_Y = Read_I2C(GYROSCOPE_Y_REGISTER_BASE);
				Gyro_Y |= (Read_I2C(GYROSCOPE_Y_REGISTER_BASE + 1) << 8);
				Gyro_Y = Gyro_Y - Gyro_Y_OFF;
				printf("%d\t", Gyro_Y);
				Gyro_Z = Read_I2C(GYROSCOPE_Z_REGISTER_BASE);
				Gyro_Z |= (Read_I2C(GYROSCOPE_Z_REGISTER_BASE + 1) << 8);
				Gyro_Z = Gyro_Z - Gyro_Z_OFF;
				printf("%d\t", Gyro_Z);
				Accel_X = Read_I2C(ACCELEROMETER_X_REGISTER_BASE);
				Accel_X |= (Read_I2C(ACCELEROMETER_X_REGISTER_BASE + 1) << 8);
				Accel_X = Accel_X - Accel_X_OFF;
				printf("%d\t", Accel_X);
				Accel_Y = Read_I2C(ACCELEROMETER_Y_REGISTER_BASE);
				Accel_Y |= (Read_I2C(ACCELEROMETER_Y_REGISTER_BASE + 1) << 8);
				Accel_Y = Accel_Y - Accel_Y_OFF;
				printf("%d\t", Accel_Y);
				Accel_Z = Read_I2C(ACCELEROMETER_Z_REGISTER_BASE);
				Accel_Z |= (Read_I2C(ACCELEROMETER_Z_REGISTER_BASE + 1) << 8);
				Accel_Z = Accel_Z - Accel_Z_OFF;
				printf("%d\r", Accel_Z);
			}
			break;
		}
	}
	
	CurrentState = NextState;
  return ReturnEvent;
}


//Function to read a byte from one of the slave's register addresses via I2C
static uint16_t Read_I2C(uint8_t IMU_Reg) 
{
	uint16_t ReturnVal = 0;
  // set transmission mode
	HWREG(I2C2_BASE + I2C_O_MSA) = IMU_SLAVE_ADDRESS;
  HWREG(I2C2_BASE + I2C_O_MSA) &= ~I2C_MSA_RS;
	// load the desired register value to be sent
  HWREG(I2C2_BASE + I2C_O_MDR) = IMU_Reg;
	// wait
	while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
	// set a multiple byte transmission
  HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
  // This is blocking, very depressing
  while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSBSY) == I2C_MCS_BUSBSY) {}
  // insert blocking delay, this is really messed up
  for(int i = 0; i < 500; i ++) {}
  // set receive mode
	HWREG(I2C2_BASE + I2C_O_MSA) = IMU_SLAVE_ADDRESS;
  HWREG(I2C2_BASE + I2C_O_MSA) |= I2C_MSA_RS;
  // set a multiple byte receive
  HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_RX;
  // Blocking code is just the pits
  while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
	// set a single byte receive
	HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_SINGLE_RX;
	// beating a dead horse something something Chris Brown
	while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
	//Read the data
	ReturnVal |= (HWREG(I2C2_BASE + I2C_O_MDR) & 0xff);
  return ReturnVal;
}

static void Send_I2C(uint8_t IMU_Reg, uint8_t IMU_Data)
{
	// set transmission mode
	HWREG(I2C2_BASE + I2C_O_MSA) = IMU_SLAVE_ADDRESS;
	HWREG(I2C2_BASE + I2C_O_MSA) &= ~I2C_MSA_RS;
	// load the desired register value to be sent
	HWREG(I2C2_BASE + I2C_O_MDR) = IMU_Reg;
	// set multiple byte transmission
	HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
	// wait
	while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
	// set multiple byte transmission
	HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_CONTINUE_TX;
	// wait for transmission
	while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
	// load data
	HWREG(I2C2_BASE + I2C_O_MDR) = IMU_Data;
	// set single byte transmission
	HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_LAST_TX;
	// wait for transmission
	while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
}

static void Offset_Init(void)
{
	Gyro_X_OFF = Read_I2C(GYROSCOPE_X_REGISTER_BASE);
	Gyro_X_OFF |= (Read_I2C(GYROSCOPE_X_REGISTER_BASE + 1) << 8);
	Gyro_Y_OFF = Read_I2C(GYROSCOPE_Y_REGISTER_BASE);
	Gyro_Y_OFF |= (Read_I2C(GYROSCOPE_Y_REGISTER_BASE + 1) << 8);
	Gyro_Z_OFF = Read_I2C(GYROSCOPE_Z_REGISTER_BASE);
	Gyro_Z_OFF |= (Read_I2C(GYROSCOPE_Z_REGISTER_BASE + 1) << 8);
	Accel_X_OFF = Read_I2C(ACCELEROMETER_X_REGISTER_BASE);
	Accel_X_OFF |= (Read_I2C(ACCELEROMETER_X_REGISTER_BASE + 1) << 8);
	Accel_Y_OFF = Read_I2C(ACCELEROMETER_Y_REGISTER_BASE);
	Accel_Y_OFF |= (Read_I2C(ACCELEROMETER_Y_REGISTER_BASE + 1) << 8);
	Accel_Z_OFF = Read_I2C(ACCELEROMETER_Z_REGISTER_BASE);
	Accel_Z_OFF |= (Read_I2C(ACCELEROMETER_Z_REGISTER_BASE + 1) << 8);
}