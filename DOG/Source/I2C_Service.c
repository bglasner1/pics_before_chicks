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
#include <math.h>
#include "inc/hw_nvic.h"

#include "Hardware.h"
#include "Constants.h"
#include "I2C_Service.h"

static uint8_t MyPriority;
static I2C_State CurrentState = I2C_Init;
static int16_t Accel_X = 0;
static int16_t Accel_Y = 0;
static int16_t Accel_Z = 0;
static int16_t Gyro_X = 0;
static int16_t Gyro_Y = 0;
static int16_t Gyro_Z = 0;
static int16_t Accel_X_OFF = 0;
static int16_t Accel_Y_OFF = 0;
static int16_t Accel_Z_OFF = 0;
static int16_t Gyro_X_OFF = 0;
static int16_t Gyro_Y_OFF = 0;
static int16_t Gyro_Z_OFF = 0;
//static int16_t thX = 0;
//static int16_t thY = 0;
//static int16_t thZ = 0;

static bool read = 0;
static uint8_t Send_Registers[2] = {ACCELEROMETER_POWER_REGISTER, GYROSCOPE_POWER_REGISTER};
static uint8_t Send_Data[2] = {ACCELEROMETER_POWER_SETTING, GYROSCOPE_POWER_SETTING};
static uint8_t Receive_Registers[12] = {GYROSCOPE_X_REGISTER_BASE, GYROSCOPE_X_REGISTER_BASE + 1, GYROSCOPE_Y_REGISTER_BASE, GYROSCOPE_Y_REGISTER_BASE + 1, GYROSCOPE_Z_REGISTER_BASE,GYROSCOPE_Z_REGISTER_BASE + 1, ACCELEROMETER_X_REGISTER_BASE, ACCELEROMETER_X_REGISTER_BASE + 1, ACCELEROMETER_Y_REGISTER_BASE, ACCELEROMETER_Y_REGISTER_BASE + 1, ACCELEROMETER_Z_REGISTER_BASE, ACCELEROMETER_Z_REGISTER_BASE + 1};
static uint8_t Receive_Data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


static uint16_t Read_I2C(uint8_t IMU_Reg);
static void Send_I2C(uint8_t IMU_Reg, uint8_t IMU_Data);
static void IMU_Update(void);
//static void Filter_Data(void);

bool Init_I2C(uint8_t Priority)
{
	// set local priority
  MyPriority = Priority;
  // set timer to allow I2C to hookup
	ES_Timer_InitTimer(IMU_TIMER, I2C_DELAY_TIME);
	// state is init
	CurrentState = I2C_Init;
	return true;
}

bool Post_I2C(ES_Event ThisEvent)
{
  return ES_PostToService( MyPriority, ThisEvent);
}

ES_Event Run_I2C(ES_Event ThisEvent)
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
				//Send_I2C(GYROSCOPE_POWER_REGISTER, GYROSCOPE_POWER_SETTING);
				//Send_I2C(ACCELEROMETER_POWER_REGISTER, ACCELEROMETER_POWER_SETTING);
				
				//printf("\r\nGyro X\tGyro Y\tGyro Z\tAccel X\tAccel Y\tAccel Z\r\n");
				HWREG(I2C2_BASE + I2C_O_MDR) = 0x0F;
				HWREG(NVIC_EN2) |= BIT4HI;
				HWREG(I2C2_BASE + I2C_O_MIMR) |= I2C_MIMR_IM;
				read = 0;
				
				HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
				
				// load Register to read
			//
				
				
				// Testing
//				uint16_t WHO_AM_I = Read_I2C((uint8_t)0x0F);
//				printf("\r\nWHO AM I: %d\r\n", WHO_AM_I);
				//uint16_t Echo = Read_I2C(ACCELEROMETER_POWER_REGISTER);
				//printf("\r\nAccel power reg: %d\r\n", Echo);
				//Echo = Read_I2C(GYROSCOPE_POWER_REGISTER);
				//printf("\r\nGyro power reg: %d\r\n", Echo);
				
				
				// set IMU Timer
				//ES_Timer_InitTimer(IMU_TIMER, CALIBRATION_TIME);
				// next state is calibrate
				NextState = I2C_Calibrate;
			}
			break;
		}
		// else if state is calibrate
		case (I2C_Calibrate):
		{
			// if event is timeout
			if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == IMU_TIMER))
			{
				// set up calibration procedure
				uint8_t n = 10;
				uint32_t GX = 0;
				uint32_t GY = 0;
				uint32_t GZ = 0;
				float AX = 0;
				float AY = 0;
				float AZ = 0;
				float ANGX = 0;
				float ANGY = 0;
				float ANGZ = 0;
				
				// get averages
				for (int i = 0; i < n; i++)
				{
					IMU_Update();
					AX = Accel_X*9.81f;
					AY = Accel_Y*9.81f;
					AZ = Accel_Z*9.81f;
					GX += Gyro_X;
					GY += Gyro_Y;
					GZ += Gyro_Z;
					ANGX += atan2(AZ, AY)*180/PI;
					ANGY += atan2(AZ, AX)*180/PI;
					ANGZ += atan2(AX, AY)*180/PI;
				}
				
				Accel_X_OFF = AX/n;
				Accel_Y_OFF = AY/n;
				Accel_Z_OFF = AZ/n;
				Gyro_X_OFF = GX/n;
				Gyro_Y_OFF = GY/n;
				Gyro_Z_OFF = GZ/n;
//				thX = ANGX/n;
//				thY = ANGY/n;
//				thZ = ANGZ/n;
				
				// set IMU poll timer
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
				IMU_Update();
				//printf("%d\t", Gyro_X);
				//printf("%d\t", Gyro_Y);
				//printf("%d\t", Gyro_Z);
				//printf("%d\t", Accel_X);
				//printf("%d\t", Accel_Y);
				//printf("%d\r", Accel_Z);
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
  while((HWREG(I2C2_BASE + I2C_O_MCS) & I2C_MCS_BUSY) == I2C_MCS_BUSY) {}
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

static void IMU_Update(void)
{
	// Read all data
	Gyro_X = Read_I2C(GYROSCOPE_X_REGISTER_BASE);
	Gyro_X |= (Read_I2C(GYROSCOPE_X_REGISTER_BASE + 1) << 8);
	Gyro_X = Gyro_X - Gyro_X_OFF;
	Gyro_Y = Read_I2C(GYROSCOPE_Y_REGISTER_BASE);
	Gyro_Y |= (Read_I2C(GYROSCOPE_Y_REGISTER_BASE + 1) << 8);
	Gyro_Y = Gyro_Y - Gyro_Y_OFF;
	Gyro_Z = Read_I2C(GYROSCOPE_Z_REGISTER_BASE);
	Gyro_Z |= (Read_I2C(GYROSCOPE_Z_REGISTER_BASE + 1) << 8);
	Gyro_Z = Gyro_Z - Gyro_Z_OFF;
	Accel_X = Read_I2C(ACCELEROMETER_X_REGISTER_BASE);
	Accel_X |= (Read_I2C(ACCELEROMETER_X_REGISTER_BASE + 1) << 8);
	Accel_X = Accel_X - Accel_X_OFF;
	Accel_Y = Read_I2C(ACCELEROMETER_Y_REGISTER_BASE);
	Accel_Y |= (Read_I2C(ACCELEROMETER_Y_REGISTER_BASE + 1) << 8);
	Accel_Y = Accel_Y - Accel_Y_OFF;
	Accel_Z = Read_I2C(ACCELEROMETER_Z_REGISTER_BASE);
	Accel_Z |= (Read_I2C(ACCELEROMETER_Z_REGISTER_BASE + 1) << 8);
	Accel_Z = Accel_Z - Accel_Z_OFF;
}

//get the upper 8 bits of the X acceleration data
uint8_t getAccelX_MSB(void)
{
	uint8_t AccelX_MSB;
	//to get the upper 8 bits, bit shift 8 times to the right
	AccelX_MSB = (Accel_X >> 8);
	//return the X MSB byte
	return AccelX_MSB;
}

//get the lower 8 bits of the X acceleration data
uint8_t getAccelX_LSB(void)
{
	uint8_t AccelX_LSB;
	//to get the lower 8 bits, and with 0xff
	AccelX_LSB = (Accel_X & 0xff);
	//return the X LSB byte
	return AccelX_LSB;
}

//get the upper 8 bits of the Y acceleration data
uint8_t getAccelY_MSB(void)
{
	uint8_t AccelY_MSB;
	//to get the upper 8 bits, bit shift 8 times to the right
	AccelY_MSB = (Accel_Y >> 8);
	//return the Y MSB byte
	return AccelY_MSB;
}

//get the lower 8 bits of the Y acceleration data
uint8_t getAccelY_LSB(void)
{
	uint8_t AccelY_LSB;
	//to get the lower 8 bits, and with 0xff
	AccelY_LSB = (Accel_Y & 0xff);
	//return the Y LSB byte
	return AccelY_LSB;	
}

//get the upper 8 bits of the Z acceleration data
uint8_t getAccelZ_MSB(void)
{
	uint8_t AccelZ_MSB;
	//to get the upper 8 bits, bit shift 8 times to the right
	AccelZ_MSB = (Accel_Z >> 8);
	//return the Z MSB byte
	return AccelZ_MSB;
}

//get the lower 8 bits of the Z acceleration data
uint8_t getAccelZ_LSB(void)
{
	uint8_t AccelZ_LSB;
	//to get the lower 8 bits, and with 0xff
	AccelZ_LSB = (Accel_Z & 0xff);
	//return the Z LSB byte
	return AccelZ_LSB;	
}

//get the upper 8 bits of the X gyro data
uint8_t getGyroX_MSB(void)
{
	uint8_t GyroX_MSB;
	//to get the upper 8 bits, bit shift 8 times to the right
	GyroX_MSB = (Gyro_X >> 8);
	//return the X MSB byte
	return GyroX_MSB;
}

//get the lower 8 bits of the X gyro data
uint8_t getGyroX_LSB(void)
{
	uint8_t GyroX_LSB;
	//to get the lower 8 bits, and with 0xff
	GyroX_LSB = (Gyro_X & 0xff);
	//return the X LSB byte
	return GyroX_LSB;	
}

//get the upper 8 bits of the Y gyro data
uint8_t getGyroY_MSB(void)
{
	uint8_t GyroY_MSB;
	//to get the upper 8 bits, bit shift 8 times to the right
	GyroY_MSB = (Gyro_Y >> 8);
	//return the Y MSB byte
	return GyroY_MSB;
}

//get the lower 8 bits of the Y gyro data
uint8_t getGyroY_LSB(void)
{
	uint8_t GyroY_LSB;
	//to get the lower 8 bits, and with 0xff
	GyroY_LSB = (Gyro_Y & 0xff);
	//return the X LSB byte
	return GyroY_LSB;
}

//get the upper 8 bits of the Gyro Z data
uint8_t getGyroZ_MSB(void)
{
	uint8_t GyroZ_MSB;
	//to get the upper 8 bits, bit shift 8 times to the right
	GyroZ_MSB = (Gyro_Z >> 8);
	//return the Z MSB byte
	return GyroZ_MSB;
}

//get the lower 8 bits of the gyro Z data
uint8_t getGyroZ_LSB(void)
{
	uint8_t GyroZ_LSB;
	//to get the lower 8 bits, and with 0xff
	GyroZ_LSB = (Gyro_Z & 0xff);
	//return the X LSB byte
	return GyroZ_LSB;	
}

//static void Filter_Data(void)
//{
//	uint8_t n = 10;
//				uint32_t GX = 0;
//				uint32_t GY = 0;
//				uint32_t GZ = 0;
//				float AX = Accel_X*9.81f;
//				float AY = Accel_Y*9.81f;
//				float AZ = Accel_Z*9.81f;
//				float ANGX = thX + Gyro_X*((float)IMU_POLL_TIME/1000)/1000000.0f;
//				float ANGY = thY + Gyro_Y*((float)IMU_POLL_TIME/1000)/1000000.0f;
//				float ANGZ = thZ + Gyro_Z*((float)IMU_POLL_TIME/1000)/1000000.0f;
//}

void I2C_ISR(void)
{
	static uint8_t Read_Index = 0;
	static uint8_t Send_Index = 0;
	static uint8_t Sends_Left = 1;
	static uint8_t Reads_Left = 11;
	//clear the source of the interrupt
	HWREG(I2C2_BASE + I2C_O_MICR) = I2C_MICR_IC;
	//if read is set
	if (read == 1)
	{
		// if index is 0
		if (Read_Index == 0)
		{
			for (int i = 0; i<400; i++);
			// set addr to read
			HWREG(I2C2_BASE + I2C_O_MSA) = IMU_SLAVE_ADDRESS;
			HWREG(I2C2_BASE + I2C_O_MSA) |= I2C_MSA_RS;
			// load START RX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_SINGLE_RX;
			// increment index
			Read_Index ++;
		}
		// else if index is 1
		else if (Read_Index == 1)
		{
			// read data from buffer
			//Receive_Data[Reads_Left] = (HWREG(I2C2_BASE + I2C_O_MDR) & 0xff);
			uint8_t Reader = (HWREG(I2C2_BASE + I2C_O_MDR) & 0xff);
			printf("\r\nreader %d\r\n", Reader);
			printf("gyro power: %d", GYROSCOPE_POWER_SETTING);
			Read_Index ++;
						// if reads left is 0
//			if (Reads_Left == 0)
//			{
//				// update Accel/Gyro vals
//				Gyro_X = ((uint16_t)(Receive_Data[0]) | ((uint16_t)Receive_Data[1] << 8)) - Gyro_X_OFF;
//				Gyro_Y = ((uint16_t)(Receive_Data[2]) | ((uint16_t)Receive_Data[3] << 8)) - Gyro_Y_OFF;
//				Gyro_Z = ((uint16_t)(Receive_Data[4]) | ((uint16_t)Receive_Data[5] << 8)) - Gyro_Z_OFF;
//				Accel_X = ((uint16_t)(Receive_Data[6]) | ((uint16_t)Receive_Data[7] << 8)) - Accel_X_OFF;
//				Accel_Y = ((uint16_t)(Receive_Data[8]) | ((uint16_t)Receive_Data[9] << 8)) - Accel_Y_OFF;
//				Accel_Z = ((uint16_t)(Receive_Data[10]) | ((uint16_t)Receive_Data[11] << 8)) - Accel_Z_OFF;
//				printf("%d\t", Gyro_X);
//				printf("%d\t", Gyro_Y);
//				printf("%d\t", Gyro_Z);
//				printf("%d\t", Accel_X);
//				printf("%d\t", Accel_Y);
//				printf("%d\r", Accel_Z);
//				// reset reads left
//				Reads_Left = 12;
//			}
//			// decrement Reads left
//			Reads_Left --;
//			// reset index to 0
//			Read_Index = 0;
//			// start next read
//			// set addr to send
//			HWREG(I2C2_BASE + I2C_O_MSA) = IMU_SLAVE_ADDRESS;
//			HWREG(I2C2_BASE + I2C_O_MSA) &= ~I2C_MSA_RS;
//			// load START TX
//			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
		}
	}
	// else if not read (send)
	else if (read == 0)
	{
		// if send index is 0
		if (Send_Index == 0)
		{
			// load Register
			HWREG(I2C2_BASE + I2C_O_MDR) = Send_Registers[Sends_Left];
			// load CONTINUE TX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_CONTINUE_TX;
			// increment send index
			Send_Index ++;
		}
		// else if send index is 1
		else if (Send_Index == 1)
		{
			// load Data
			HWREG(I2C2_BASE + I2C_O_MDR) = Send_Data[Sends_Left];
			// load LAST TX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_LAST_TX;
			// increment send index
			Send_Index ++;
		}
		// else if send index is 2
		else if (Send_Index == 2)
		{
			// if sends left is 1
			if (Sends_Left != 0)
			{
				// decrement sends left
				Sends_Left --;
			}
			// else if sends left is 0
			else if (Sends_Left == 0)
			{
				// set read
				read = 1;
				// load register to read
				//HWREG(I2C2_BASE + I2C_O_MDR) = Receive_Registers[Reads_Left];
				HWREG(I2C2_BASE + I2C_O_MDR) = GYROSCOPE_POWER_REGISTER;
			}
			// set send index to 0
			Send_Index = 0;
			// load START TX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
		}
	}
}