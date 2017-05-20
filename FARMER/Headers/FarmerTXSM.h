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
void disableTransmit(void);
void setUnpair( void );
void setPair( void );
void setFarmerDataHeader(uint8_t Header);
void setDestDogAddress(uint8_t AddrMSB, uint8_t AddrLSB);
void setDogTag(uint8_t TagNumber);
void setDriveCtrl(uint8_t CtrlByte);
void setSteeringCtrl(uint8_t CtrlByte);
void setDigitalCtrl(uint8_t CtrlByte);


#endif /* FarmerTXSM_H */
							
							
							