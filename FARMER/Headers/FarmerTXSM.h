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
//void setUnpair( void );
//void setPair( void );
void setFarmerDataHeader(uint8_t Header);
void setDestDogAddress(uint8_t AddrMSB, uint8_t AddrLSB);
void setDogTag(uint8_t TagNumber);
void EnableReverse(void);
void DisableReverse(void);
void EnableLeftBrake(void);
void DisableLeftBrake(void);
void EnableRightBrake(void);
void DisableRightBrake(void);
void TogglePeripheral(void);
uint8_t getDestAddrMSB(void);
uint8_t getDestAddrLSB(void);
void resetEncryptionIndex(void);
uint8_t getEncryptionKeyIndex(void);
void clearControls(void);


#endif /* FarmerTXSM_H */
							
							
			



