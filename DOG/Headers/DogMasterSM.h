/****************************************************************************
 
  Header file for DogMasterSM

 ****************************************************************************/

#ifndef DOG_MASTER_SM_H
#define DOG_MASTER_SM_H



typedef enum {Unpaired, Wait2Pair, Paired} DogMasterState_t;


bool InitDogMasterSM(uint8_t Priority);
bool PostDogMasterSM(ES_Event ThisEvent);
ES_Event RunDogMasterSM(ES_Event ThisEvent);

uint8_t getHardwareDogTag( void );

#endif
