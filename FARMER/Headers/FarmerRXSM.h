/****************************************************************************
 
  Header file for FarmerRXSM

 ****************************************************************************/

#ifndef FarmerRXSM_H
#define FarmerRXSM_H

#include "ES_Configure.h"
#include "ES_Types.h"

// Public Function Prototypes

typedef enum {WaitForFirstByte,
							WaitForMSBLen,
							WaitForLSBLen,
							AcquireData
							} FarmerRX_State_t;

bool InitFarmerRXSM ( uint8_t Priority );
bool PostFarmerRXSM( ES_Event ThisEvent );
void setPair(void);
void setUnpair(void);
ES_Event RunFarmerRXSM( ES_Event ThisEvent );
void FarmerRX_ISR( void );
void RXTX_ISR( void );


#endif /* FarmerRXSM_H */

