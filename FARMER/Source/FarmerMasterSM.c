bool InitFarmerMasterSM(uint8_t Priority)
{
	// state is unpaired
	// post entry event to self
	// set priority
}

bool PostFarmerMasterSM(ES_Event ThisEvent)
{
	// post event
}

ES_Event RunFarmerMasterSM(ES_Event ThisEvent)
{
	// set return event
	// next state is current state
	// loop through states
		// if current state is unpaired
			// if event is entry
				// set the LED blink timer
				// set blinker
				// call LED function
			// else if event is timeout
				// post entry event to self
			// else if event is right button down
				// increment the DOG selector
			// else if the event is speech detected
				// set request pair in FARMER_TX_SM with DOG
				// set desired dog in FARMER_RX_SM
				// next state is Wait2Pair
				// post entry event to self
				// enable transmit in FarmerTX
				// set message to REQ_2_PAIR in FarmerTX
		// else if current state is Wait2Pair
			// if event is entry
				// set the LED blink timer
				// toggle the Blink LED
			// else if event is timeout
				// post entry event to self
			// else if event is Lost connection
				// disable transmit in FarmerTX
				// next state is Unpaired
				// post entry event to self
			// else if event is ES_CONNECTION_SUCCESSFUL
				// clear blinker
				// Call LED function
				// set paired in TX and RX
				// Set message to ENCR_KEY in FarmerTx
				// Next state is Wait2Encrypt
		// else if current state is Wait2Encrypt
			// if event is ES_PAIR_SUCCESSFUL
				// next state is Paired
				// Set message to CTRL
				// clear blinker
				// Call LED function
				// set paired in TX and RX
			// else if event is Lost connection
				// next state is Unpaired
				// disable transmit in FarmerTX
				// post entry event to self
		// else if state is paired
			// if event is right button down
				// set right brake active in TX
			// else if event is right button up
				// set right brake inactive in TX
			// else if event is left button down
				// set left brake active in TX
			// else if event is left button up
				// set left brake inactive in TX
			// else if event is reverse button down
				// set reverse active in TX
			// else if event is reverse button up
				// set forward active in TX
			// else if event is peripheral button down
				// toggle peripheral in tx
			// else if event is ES_RESEND_ENCRYPT
				// Next state is Wait2Encrypt
				// Set message to ENCR_KEY in FarmerTX
			// else if event is lost connection
				// set unpaired in TX and RX
				// disable transmit in FarmerTX
				// post entry event to self
				// next state is unpaired
				
		//Set current state to next state
		//return return event
}

static void LED_Setter(void)
{
	// Clear last LED that was written
	// if blinker is set
		// current LED is Yellow LED associated with the DOG selector
		// toggle Last LED state
		// write Last LED state to selected LED 
	// else
		// current LED is Green LED assosciated with the DOG selector
		// clear last LED state
		// write selected LED High
	// last LED is current LED
}