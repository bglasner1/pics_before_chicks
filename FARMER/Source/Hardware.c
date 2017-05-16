#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "TestHarnessService0.h"

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
#include "ES_ShortTimer.h"

#include "ADMulti.h"
#include "Constants.h"

static void IO_Init(void);
static void AD_Init(void);
static void UART_Init(void);

void Hardware_Init(void)
{
	IO_Init();
	AD_Init();
	UART_Init();
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

static void UART_Init(void)
{
	//Enable the clock for the UART module
	HWREG(SYSCTL_RCGCUART) |= SYSCTL_RCGCUART_R1;
	
	//Wait for the UART to be ready
	while((HWREG(SYSCTL_PRUART)& SYSCTL_PRUART_R1)!=SYSCTL_PRUART_R1){}
		
	//Enable the clock to the appropriate gpio module via the RCGCGPIO - port C
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R2;
	
	//Wait for the GPIO module to be ready
	while((HWREG(SYSCTL_PRGPIO)& SYSCTL_PRGPIO_R2)!=SYSCTL_PRGPIO_R2){}
		
	//Configure the GPIO pins for in/out/drive-level/drive-type
	HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= (GPIO_PIN_4 | GPIO_PIN_5);
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) |= GPIO_PIN_5;
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) &= ~GPIO_PIN_4;
		
	//Select the Alternate function for the UART pins
	HWREG(GPIO_PORTC_BASE+GPIO_O_AFSEL) |= (BIT4HI | BIT5HI);
		
	//Configure the PMCn fields in the GPIOPCTL register to assign the UART pins
	HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) & 0Xfff0ffff) + (RX_ALT_FUNC<<(RX_PIN*BITS_PER_NIBBLE));
	HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) & 0Xff0fffff) + (TX_ALT_FUNC<<(TX_PIN*BITS_PER_NIBBLE));
		
	//Disable the UART by clearing the UARTEN bit in the UARTCTL register
	HWREG(UART1_BASE+UART_O_CTL) = HWREG(UART1_BASE + UART_O_CTL) & ~UART_CTL_UARTEN;
		
	//Write the integer portion of the BRD
	HWREG(UART1_BASE + UART_O_IBRD) = BAUD_RATE_INT;
		
	//Write the fraction portion of the BRD
	HWREG(UART1_BASE + UART_O_FBRD) = BAUD_RATE_FRAC;
	
	//Write the desired serial parameters
	HWREG(UART1_BASE + UART_O_LCRH) = HWREG(UART1_BASE + UART_O_LCRH) | UART_LCRH_WLEN_8;
	
	//Enable RX and TX interrupts in mask
	HWREG(UART1_BASE + UART_O_IM) = HWREG(UART1_BASE + UART_O_IM) | UART_IM_RXIM;
	
	//Configure the UART operation
	//Enable the UART
	HWREG(UART1_BASE + UART_O_CTL) = HWREG(UART1_BASE + UART_O_CTL) | UART_CTL_UARTEN;
	
	//Enable interrupt in the NVIC
	HWREG(NVIC_EN0) |= BIT6HI;
	
	//Enable interrupts globally
	__enable_irq();
	
	//Print successful initialization
	printf("UART 1 Successfully Initialized! :)\r\n");
}


