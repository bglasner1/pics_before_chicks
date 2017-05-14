//#define TEST

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
#include "driverlib/pwm.h"
#include "driverlib/i2c.h"
#include "inc/hw_pwm.h"
#include "inc/hw_i2c.h"
#include "inc/hw_nvic.h"


#include "termio.h"

#define clrScrn() 	printf("\x1b[2J")
#define goHome()	printf("\x1b[1,1H")
#define clrLine()	printf("\x1b[K")

#include "ADMulti.h"
#include "Constants.h"

static uint8_t LastDirThrust = FORWARD;

static void IO_Init(void);
static void AD_Init(void);
static void PWM_Init(void);
static void I2C_Init(void);

void Hardware_Init(void)
{
	IO_Init();
	AD_Init();
	PWM_Init();
	I2C_Init();
}

static void IO_Init(void)
{
	// connect clock to ports B
	HWREG(SYSCTL_RCGCGPIO) |= (SYSCTL_RCGCGPIO_R1);
	// wait for clock to connect to ports B and F
	while ((HWREG(SYSCTL_PRGPIO) & (SYSCTL_PRGPIO_R1)) != (SYSCTL_PRGPIO_R1)) {}
	// digitally enable IO pins
	HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) |= (THRUST_FAN_DIR_B);
	// set direction of IO pins
	HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) |= (THRUST_FAN_DIR_B);
	
}

static void AD_Init(void)
{
	// Connect clock to port E
	HWREG(SYSCTL_RCGCGPIO)|=SYSCTL_RCGCGPIO_R4;
	// wait for clock to connect to port E
	while((HWREG(SYSCTL_PRGPIO)& SYSCTL_PRGPIO_R4)!=SYSCTL_PRGPIO_R4){}
	// digitally enable Anaolog Pins (I realize this doesn't make any sense, it's 2 am leave me alone)
	HWREG(GPIO_PORTE_BASE + GPIO_O_DEN) |= (DOG_TAG_E);
	// set direction of Analog Pins
	HWREG(GPIO_PORTE_BASE + GPIO_O_DIR) &= ~(DOG_TAG_E);
	ADC_MultiInit(NUMBER_OF_ANALOG_PINS);
}

static void PWM_Init(void)
{
	
	// Enable the clock to the PWM Module
	HWREG(SYSCTL_RCGCPWM) |= (SYSCTL_RCGCPWM_R0);
	while ((HWREG(SYSCTL_PRPWM) & (SYSCTL_PRPWM_R0)) != (SYSCTL_PRPWM_R0)) {}
	// Enable the clock to Port B
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R1;
	while ((HWREG(SYSCTL_PRGPIO) & (SYSCTL_PRGPIO_R0 | SYSCTL_PRGPIO_R1)) != (SYSCTL_PRGPIO_R0 | SYSCTL_PRGPIO_R1)) {}
	// digitially enable the PWM pins
	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (LEFT_SERVO_PIN_B | RIGHT_SERVO_PIN_B | INDICATOR_PIN_B | THRUST_FAN_PWM_PIN_B);
	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) |= (LEFT_SERVO_PIN_B | RIGHT_SERVO_PIN_B | INDICATOR_PIN_B | THRUST_FAN_PWM_PIN_B);
	// Select the system clock/32
	HWREG(SYSCTL_RCC) = (HWREG(SYSCTL_RCC) & ~SYSCTL_RCC_PWMDIV_M) | (SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_32);
	// Disable the PWM generator while initializing
	HWREG(PWM0_BASE+PWM_O_0_CTL) = 0;
	HWREG(PWM0_BASE+PWM_O_1_CTL) = 0;
	// Set initial gernator values: motors should be stopped, servos at idle
	HWREG(PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ZERO;
	HWREG(PWM0_BASE+PWM_O_0_GENB) = PWM_0_GENB_ACTZERO_ZERO;	
	HWREG(PWM0_BASE+PWM_O_1_GENA) = GenA_1_Normal;
	HWREG(PWM0_BASE+PWM_O_1_GENB) = GenB_1_Normal;
	// Set the load to ½ the desired period since going up and down
	HWREG(PWM0_BASE+PWM_O_0_LOAD) = ((MOTOR_PWM_PERIOD) >> 1);
	HWREG(PWM0_BASE+PWM_O_1_LOAD) = ((SERVO_PWM_PERIOD) >> 1);
	// Set the initial duty cycle on the servos
	HWREG(PWM0_BASE+PWM_O_1_CMPA) = LEFT_SERVO_IDLE_DUTY;
	HWREG(PWM0_BASE+PWM_O_1_CMPB) = RIGHT_SERVO_IDLE_DUTY;
	// Enable the PWM outputs
	HWREG(PWM0_BASE+PWM_O_ENABLE) |= (PWM_ENABLE_PWM0EN | PWM_ENABLE_PWM1EN | PWM_ENABLE_PWM2EN | PWM_ENABLE_PWM3EN);
	// Select the alternate function for PWM Pins
	HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= (LEFT_SERVO_PIN_B | RIGHT_SERVO_PIN_B | INDICATOR_PIN_B | THRUST_FAN_PWM_PIN_B);
	// Choose to map PWM to those pins
	HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) & PWM_PIN_M) + (4<<(LEFT_SERVO_BIT*BitsPerNibble)) + (4<<(RIGHT_SERVO_BIT*BitsPerNibble)) + (4<<(INDICATOR_BIT*BitsPerNibble)) + (4<<(THRUST_FAN_PWM_BIT*BitsPerNibble));
	// Set the up/down count mode
	// Enable the PWM generator
	// Make generator updates locally synchronized to zero count
	HWREG(PWM0_BASE+PWM_O_0_CTL) = (PWM_0_CTL_MODE | PWM_0_CTL_ENABLE | PWM_0_CTL_GENAUPD_LS | PWM_0_CTL_GENBUPD_LS);
	HWREG(PWM0_BASE+PWM_O_1_CTL) = (PWM_1_CTL_MODE | PWM_1_CTL_ENABLE | PWM_1_CTL_GENAUPD_LS | PWM_0_CTL_GENBUPD_LS);
	
}

void SetDutyThrustFan(uint8_t duty) 
{
	
	// Motor starts at rest
	static bool restoreMotor = true;
	
	// New Value for comparator to set duty cycle
	static uint32_t newCmp;
	if (LastDirThrust == REVERSE) duty = 100 - duty;
	// set new comparator value based on duty cycle
	newCmp = HWREG(PWM0_BASE + PWM_O_0_LOAD)*(100-duty)/100;
	if (duty == 100 | duty == 0) 
	{
		restoreMotor = true;
		if (duty == 100) 
		{
			// To program 100% DC, simply set the action on Zero to set the output to one
			HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ONE;
		} 
		else 
		{
			// To program 0% DC, simply set the action on Zero to set the output to zero
			HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ZERO;
		}
	} 
	else 
	{
		// if returning from 0 or 100
		if (restoreMotor) 
		{
			restoreMotor = false;
			// restore normal operation
			if (LastDirThrust == FORWARD)
			{
				HWREG(PWM0_BASE+PWM_O_0_GENA) = GenA_0_Normal;
			}
			else if (LastDirThrust == REVERSE)
			{
				HWREG(PWM0_BASE+PWM_O_0_GENA) = GenA_0_Invert;
			}
		}
		// write new comparator value to register
		HWREG( PWM0_BASE+PWM_O_0_CMPA) = newCmp;
	}
	
}

void SetDutyIndicator(uint8_t duty) 
{
	
	// Motor starts at rest
	static bool restoreIndicator = true;
	
	// New Value for comparator to set duty cycle
	static uint32_t newCmp;
	// set new comparator value based on duty cycle
	newCmp = HWREG(PWM0_BASE + PWM_O_0_LOAD)*(100-duty)/100;
	if (duty == 100 | duty == 0) 
	{
		restoreIndicator = true;
		if (duty == 100) 
		{
			// To program 100% DC, simply set the action on Zero to set the output to one
			HWREG(PWM0_BASE+PWM_O_0_GENB) = PWM_0_GENB_ACTZERO_ONE;
		} 
		else 
		{
			// To program 0% DC, simply set the action on Zero to set the output to zero
			HWREG(PWM0_BASE+PWM_O_0_GENB) = PWM_0_GENB_ACTZERO_ZERO;
		}
	} 
	else 
	{
		// if returning from 0 or 100
		if (restoreIndicator) 
		{
			restoreIndicator = false;
			// restore normal operation
			HWREG(PWM0_BASE + PWM_O_0_GENB) = GenB_0_Normal;
		}
		// write new comparator value to register
		HWREG(PWM0_BASE + PWM_O_0_CMPB) = newCmp;
	}
	
}

void SetDirectionThrust(uint8_t dir) 
{
	
	if (dir==REVERSE) {
		HWREG(PWM0_BASE + PWM_O_0_GENA) = GenA_0_Invert;
		HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS)) |= (THRUST_FAN_DIR_B);
	}
	else if (dir==FORWARD) {
		HWREG(PWM0_BASE + PWM_O_0_GENA) = GenA_0_Normal;
		HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS)) &= ~(THRUST_FAN_DIR_B);
	}
	LastDirThrust=dir;
	
}

void SetLeftBrakePosition(uint16_t position)
{
	
	// New Value for comparator to set duty cycle
	uint32_t newCmp = HWREG(PWM0_BASE+PWM_O_1_LOAD)*(12500-position)/12500;
	// write new comparator value to register
	HWREG(PWM0_BASE+PWM_O_1_CMPA) = newCmp;

}

void SetRightBrakePosition(uint16_t position)
{
	
	// New Value for comparator to set duty cycle
	uint32_t newCmp = HWREG(PWM0_BASE+PWM_O_1_LOAD)*(12500-position)/12500;
	// write new comparator value to register
	HWREG(PWM0_BASE+PWM_O_1_CMPB) = newCmp;

}

uint8_t ReadDOGTag(void)
{
	uint32_t TagVal[1];
	ADC_MultiRead(TagVal);
	if (TagVal[0] < DOG_3_THRESHOLD)
	{
		return DOG_3;
	}
	else if (TagVal[0] < DOG_2_THRESHOLD)
	{
		return DOG_2;
	}
	else
	{
		return DOG_1;
	}
}

static void I2C_Init(void)
{
	// enable the I2C clock for I2C module 2
	HWREG(SYSCTL_RCGCI2C) |= SYSCTL_RCGCI2C_R2;
	while ((HWREG(SYSCTL_PRI2C) & SYSCTL_PRI2C_R2) != SYSCTL_PRI2C_R2) {}
	// enable clock to GPIO pins on I2C2 (E4, E5)
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R4;
	while ((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R4) != SYSCTL_PRGPIO_R4) {}
	//enable internal pullups
	HWREG(GPIO_PORTE_BASE + GPIO_O_PUR) |= (I2C_SDA_PIN | I2C_SCL_PIN);
	// digitally enable maybe?
	HWREG(GPIO_PORTE_BASE + GPIO_O_DEN) |= (I2C_SDA_PIN | I2C_SCL_PIN);
	// select alternate functions for B2, B3
	HWREG(GPIO_PORTE_BASE + GPIO_O_AFSEL) |= (I2C_SDA_PIN | I2C_SCL_PIN);
	// set SDA to Open Drain
	HWREG(GPIO_PORTE_BASE + GPIO_O_ODR) |= I2C_SDA_PIN;
	// select I2C function
	HWREG(GPIO_PORTE_BASE + GPIO_O_PCTL) = ((HWREG(GPIO_PORTE_BASE + GPIO_O_PCTL) & I2C_PIN_M) | ((3 << (I2C_SDA_BIT*BitsPerNibble)) | (3 << (I2C_SCL_BIT*BitsPerNibble))));
	// initialize the TIVA as Master
	HWREG(I2C2_BASE + I2C_O_MCR) |= I2C_MCR_MFE;
	// set the SCL clock (there is a fancy equation, I'm just using the provided 10KBPS val given)
	HWREG(I2C2_BASE + I2C_O_MTPR) = ((HWREG(I2C2_BASE + I2C_O_MTPR) & ~(I2C_MTPR_TPR_M)) | I2C_COMM_SPEED);
	// Load Slave address
	HWREG(I2C2_BASE + I2C_O_MSA) |= IMU_SLAVE_ADDRESS;
}


#ifdef TEST

int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
			| SYSCTL_XTAL_16MHZ);
	TERMIO_Init();
	clrScrn();
	
	Hardware_Init();
	
	while(1)
	{
		uint8_t Dog = ReadDOGTag();
	}
	
	return 0;
}

#endif
