/****************************************************************************
 
  Header file for DogTXSM

 ****************************************************************************/

#ifndef DogTXSM_H
#define DogTXSM_H

#include "ES_Configure.h"
#include "ES_Types.h"

// Public Function Prototypes

typedef enum {Waiting2Transmit,
							Transmit
							} DogTX_State_t;

bool InitDogTXSM ( uint8_t Priority );
bool PostDogTXSM( ES_Event ThisEvent );
ES_Event RunDogTXSM( ES_Event ThisEvent );
void DogTX_ISR( void );
void sendToPIC(uint8_t value);
void setDogDataHeader(uint8_t Header);
void setDestFarmerAddress(uint8_t AddrMSB, uint8_t AddrLSB);
uint8_t getDestFarmerAddressMSB( void );
uint8_t getDestFarmerAddressLSB( void );


#endif /* DogTXSM_H */
							
							

							
							
							
