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
							AcquireData,
							Waiting2Rec,
							Receive
							} DogRX_State_t;

bool InitDogRXSM ( uint8_t Priority );
bool PostDogRXSM( ES_Event ThisEvent );
ES_Event RunDogRXSM( ES_Event ThisEvent );
void DogRX_ISR( void );
void RXTX_ISR( void );
uint8_t getHeader( void );
uint8_t getAPI( void );
uint8_t getSoftwareDogTag( void );
void ResetEncr( void );
uint8_t getLSBAddress( void );
uint8_t getMSBAddress( void );
void DecryptData( void );
void StoreEncr( void );
uint8_t getPerData( void );
uint8_t getBrakeData( void );
uint8_t getMoveData( void );
uint8_t getTurnData( void );
void ClearDataArray( void );	
							

#endif /* DogRXSM_H */

