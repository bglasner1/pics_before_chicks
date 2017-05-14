/****************************************************************************
 
  Header file for FarmerTXSM

 ****************************************************************************/

#ifndef FarmerTXSM_H
#define FarmerTXSM_H

#include "ES_Configure.h"
#include "ES_Types.h"

// Public Function Prototypes

typedef enum {Waiting2Transmit,
							Transmit
							} FarmerTX_State_t;

bool InitFarmerTXSM ( uint8_t Priority );
bool PostFarmerTXSM( ES_Event ThisEvent );
ES_Event RunFarmerTXSM( ES_Event ThisEvent );
void FarmerTX_ISR( void );
void enableTransmit( void );


#endif /* FarmerTXSM_H */
							
							
							