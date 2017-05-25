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
static uint8_t Send_Registers[1] = {POWER_REGISTER};
static uint8_t Send_Data[1] = {POWER_SETTING};
static uint8_t Receive_Registers[12] = {GYROSCOPE_X_REGISTER_BASE, GYROSCOPE_X_REGISTER_BASE - 1, GYROSCOPE_Y_REGISTER_BASE, GYROSCOPE_Y_REGISTER_BASE - 1, GYROSCOPE_Z_REGISTER_BASE,GYROSCOPE_Z_REGISTER_BASE - 1, ACCELEROMETER_X_REGISTER_BASE, ACCELEROMETER_X_REGISTER_BASE - 1, ACCELEROMETER_Y_REGISTER_BASE, ACCELEROMETER_Y_REGISTER_BASE - 1, ACCELEROMETER_Z_REGISTER_BASE, ACCELEROMETER_Z_REGISTER_BASE - 1};
static uint16_t Receive_Data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
				printf("\r\nGyro X\tGyro Y\tGyro Z\tAccel X\tAccel Y\tAccel Z\r\n");
				// initialize Gyro/accelerometer power settings
				HWREG(I2C2_BASE + I2C_O_MDR) = Send_Registers[0];
				HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
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
				
				
				// get offsets
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
				// send IMU data
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
	static uint8_t Sends_Left = 0;
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
			Receive_Data[Reads_Left] = (HWREG(I2C2_BASE + I2C_O_MDR) & 0xff);
			// if reads left is 0
			if (Reads_Left == 0)
			{
				// update Accel/Gyro vals
				Gyro_X = ((Receive_Data[0]) | (Receive_Data[1] << 8)) - Gyro_X_OFF;
				Gyro_Y = ((Receive_Data[2]) | (Receive_Data[3] << 8)) - Gyro_Y_OFF;
				Gyro_Z = ((Receive_Data[4]) | (Receive_Data[5] << 8)) - Gyro_Z_OFF;
				Accel_X = ((Receive_Data[6]) | (Receive_Data[7] << 8)) - Accel_X_OFF;
				Accel_Y = ((Receive_Data[8]) | (Receive_Data[9] << 8)) - Accel_Y_OFF;
				Accel_Z = ((Receive_Data[10]) | (Receive_Data[11] << 8)) - Accel_Z_OFF;
				//printf("%d\t", Gyro_X);
				//printf("%d\t", Gyro_Y);
				//printf("%d\t", Gyro_Z);
//				printf("%d\t", Receive_Data[6]);
//				printf("%d\t", Receive_Data[8]);
//				printf("%d\t", Receive_Data[10]);
//				printf("%d\t", Receive_Data[7]);
//				printf("%d\t", Receive_Data[9]);
//				printf("%d\r", Receive_Data[11]);
				// reset reads left
				Reads_Left = 12;
			}
			// decrement Reads left
			Reads_Left --;
			// reset index to 0
			Read_Index = 0;
			// start next read
			// set addr to send
			HWREG(I2C2_BASE + I2C_O_MSA) = IMU_SLAVE_ADDRESS;
			HWREG(I2C2_BASE + I2C_O_MSA) &= ~I2C_MSA_RS;
			// load register to read
			HWREG(I2C2_BASE + I2C_O_MDR) = Receive_Registers[Reads_Left];
			// load START TX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
		}
	}
	// else if not read (send)
	else if (read == 0)
	{
		// if send index is 0
		if (Send_Index == 0)
		{
			// load Data
			HWREG(I2C2_BASE + I2C_O_MDR) = Send_Data[Sends_Left];
			// load LAST TX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_LAST_TX;
			// increment send index
			Send_Index ++;
		}
		// else if send index is 2
		else if (Send_Index == 1)
		{
			// if sends left is 1
			if (Sends_Left != 0)
			{
				// decrement sends left
				Sends_Left --;
				// load register to write
				HWREG(I2C2_BASE + I2C_O_MDR) = Send_Registers[Sends_Left];
			}
			// else if sends left is 0
			else if (Sends_Left == 0)
			{
				// set read
				read = 1;
				// load register to read
				HWREG(I2C2_BASE + I2C_O_MDR) = Receive_Registers[Reads_Left];
			}
			// set send index to 0
			Send_Index = 0;
			// load START TX
			HWREG(I2C2_BASE + I2C_O_MCS) = I2C_MCS_START_TX;
		}
	}
}
