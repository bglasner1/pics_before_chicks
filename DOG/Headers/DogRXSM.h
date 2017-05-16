/****************************************************************************
 
  Header file for DogRXSM

 ****************************************************************************/

#ifndef DogRXSM_H
#define DogRXSM_H

#include "ES_Configure.h"
#include "ES_Types.h"

// Public Function Prototypes

typedef enum {WaitForFirstByte,
							WaitForMSBLen,
							WaitForLSBLen,
							AcquireData
							} DogRX_State_t;

bool InitDogRXSM ( uint8_t Priority );
bool PostDogRXSM( ES_Event ThisEvent );
ES_Event RunDogRXSM( ES_Event ThisEvent );
void DogRX_ISR( void );
void RXTX_ISR( void );


#endif /* DogRXSM_H */
