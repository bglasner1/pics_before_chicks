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

static void IO_Init(void);
static void AD_Init(void);

void Hardware_Init(void)
{
	IO_Init();
	AD_Init();
}

static void IO_Init(void)
{
	// connect clock to ports B and F
	HWREG(SYSCTL_RCGCGPIO) |= (SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R3);
	// wait for clock to connect to ports B and F
	while ((HWREG(SYSCTL_PRGPIO) & (SYSCTL_PRGPIO_R1 | SYSCTL_PRGPIO_R3)) != (SYSCTL_PRGPIO_R1 | SYSCTL_PRGPIO_R3)) {}
	// digitally enable IO pins
	HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) |= (R_BUTTON_B | L_BUTTON_B);
	HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= (REVERSE_BUTTON_D | PERIPHERAL_BUTTON_D);
	// set direction of IO pins
	HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) &= ~(R_BUTTON_B | L_BUTTON_B);
	HWREG(GPIO_PORTD_BASE + GPIO_O_DIR) &= ~(REVERSE_BUTTON_D | PERIPHERAL_BUTTON_D);
	
}

static void AD_Init(void)
{
	// Connect clock to port E
	HWREG(SYSCTL_RCGCGPIO)|=SYSCTL_RCGCGPIO_R4;
	// wait for clock to connect to port E
	while((HWREG(SYSCTL_PRGPIO)& SYSCTL_PRGPIO_R4)!=SYSCTL_PRGPIO_R4){}
	// digitally enable Anaolog Pins (I realize this doesn't make any sense, it's 2 am leave me alone)
	HWREG(GPIO_PORTE_BASE + GPIO_O_DEN) |= (SOUND_PIN_E);
	// set direction of Analog Pins
	HWREG(GPIO_PORTE_BASE + GPIO_O_DIR) &= ~(SOUND_PIN_E);
	ADC_MultiInit(NUMBER_OF_ANALOG_PINS);
}
