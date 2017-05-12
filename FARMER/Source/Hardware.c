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

#define R_BUTTON_B GPIO_PIN_0
#define L_BUTTON_B GPIO_PIN_1
#define REVERSE_BUTTON_F GPIO_PIN_0
#define PERIPHERAL_BUTTON_F GPIO_PIN_1
#define ALL_BITS (0xFF >> 2)

static void IO_Init(void)
static void Analog_Init(void)

void Hardware_Init(void)
{
	
}

static void IO_Init(void)
{
	// connect clock to ports B and F
	HWREG(SYSCTL_RCGCGPIO) |= (SYSCTL_RCGCGPIO_R1 | RCGCGPIO_R5);
	// wait for clock to connect to ports B and F
	while ((HWREG(SYSCTL_PRGPIO) & (SYSCTL_PRGPIO_R1 | SYSCTL_PRGPIO_R5)) != (SYSCTL_PRGPIO_R1 | SYSCTL_PRGPIO_R5)) {}
	// digitally enable IO pins
	HWREG(GPIO_PORTB_BASE + (ALL_BITS + GPIO_O_DEN)) |= (R_BUTTON_B | L_BUTTON_B)
	HWREG(GPIO_PORTF_BASE + (ALL_BITS + GPIO_O_DEN)) |= (REVERSE_BUTTON_F | PERIPHERAL_BUTTON_F)
	// set direction of IO pins
	HWREG(GPIO_PORTB_BASE + (ALL_BITS + GPIO_O_DIR)) &= ~(R_BUTTON_B | L_BUTTON_B)
	HWREG(GPIO_PORTF_BASE + (ALL_BITS + GPIO_O_DIR)) &= ~(REVERSE_BUTTON_F | PERIPHERAL_BUTTON_F)
	
}