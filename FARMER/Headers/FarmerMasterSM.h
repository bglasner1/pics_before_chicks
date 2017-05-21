/****************************************************************************
 
  Header file for FarmerMasterSM

 ****************************************************************************/

#ifndef FARMER_MASTER_SM_H
#define FARMER_MASTER_SM_H



typedef enum {Unpaired, Wait2Pair,Wait2Encrypt, Paired} FarmerMasterState_t


bool InitFarmerMasterSM(uint8_t Priority);
bool PostFarmerMasterSM(ES_Event ThisEvent);
ES_Event RunFarmerMasterSM(ES_Event ThisEvent);


#endif
