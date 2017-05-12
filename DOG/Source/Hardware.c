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

#include "ADMulti.h"
#include "Constants.h"

static uint8_t LastDirThrust = FORWARD;
static uint8_t LastDirIndicator = FORWARD;

static void IO_Init(void);
static void AD_Init(void);
static void PWM_Init(void);

void Hardware_Init(void)
{
	IO_Init();
	AD_Init();
	PWM_Init();
}

static void IO_Init(void)
{
	// connect clock to ports B
	HWREG(SYSCTL_RCGCGPIO) |= (SYSCTL_RCGCGPIO_R1);
	// wait for clock to connect to ports B and F
	while ((HWREG(SYSCTL_PRGPIO) & (SYSCTL_PRGPIO_R1) != (SYSCTL_PRGPIO_R1) {}
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
	HWREG(PWM0_BASE+PWM_O_2_CMPA) = LEFT_SERVO_IDLE_DUTY;
	HWREG(PWM0_BASE+PWM_O_3_CMPA) = RIGHT_SERVO_IDLE_DUTY;
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
	newCmp = HWREG(PWM0_BASE+PWM_O_0_LOAD)*(100-duty)/100;
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
	if (LastDirIndicator == REVERSE) duty = 100 - duty;
	// set new comparator value based on duty cycle
	newCmp = HWREG(PWM0_BASE+PWM_O_1_LOAD)*(100-duty)/100;
	if (duty == 100 | duty == 0) 
	{
		restoreIndicator = true;
		if (duty == 100) 
		{
			// To program 100% DC, simply set the action on Zero to set the output to one
			HWREG( PWM0_BASE+PWM_O_0_GENB) = PWM_0_GENB_ACTZERO_ONE;
		} 
		else 
		{
			// To program 0% DC, simply set the action on Zero to set the output to zero
			HWREG( PWM0_BASE+PWM_O_0_GENB) = PWM_0_GENB_ACTZERO_ZERO;
		}
	} 
	else 
	{
		// if returning from 0 or 100
		if (restoreIndicator) 
		{
			restoreIndicator = false;
			// restore normal operation
			if (LastDirIndicator == FORWARD)
			{
				HWREG(PWM0_BASE+PWM_O_0_GENB) = GenB_0_Normal;
			}
			else if (LastDirIndicator == REVERSE)
			{
				HWREG(PWM0_BASE+PWM_O_0_GENB) = GenB_0_Invert;
			}
		}
		// write new comparator value to register
		HWREG( PWM0_BASE+PWM_O_0_CMPA) = newCmp;
	}
	
}

void SetDirectionMotor(uint8_t dir) 
{
	
	if (dir==REVERSE) {
		HWREG(PWM0_BASE + PWM_O_0_GENA) = GenA__0_Invert;
		HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS)) |= (THRUST_FAN_DIR_B);
	}
	else if (dir==FORWARD) {
		HWREG(PWM0_BASE + PWM_O_0_GENA) = GenA_0_Normal;
		HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + ALL_BITS)) &= ~(THRUST_FAN_DIR_B);
	}
	LastDirMotor=dir;
	
}

void SetLeftBrakePosition(uint16_t position)
{
	
	// New Value for comparator to set duty cycle
	uint32_t newCmp = HWREG(PWM0_BASE+PWM_O_2_LOAD)*(12500-position)/12500;
	// write new comparator value to register
	HWREG(PWM0_BASE+PWM_O_2_CMPA) = newCmp;

}

void SetRightBrakePosition(uint16_t position)
{
	
	// New Value for comparator to set duty cycle
	uint32_t newCmp = HWREG(PWM0_BASE+PWM_O_3_LOAD)*(12500-position)/12500;
	// write new comparator value to register
	HWREG(PWM0_BASE+PWM_O_3_CMPA) = newCmp;

}