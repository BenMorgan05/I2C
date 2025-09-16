	#include "I2C.h"


    // int _SDA;
    // int _SCL;
    // int _frequency;
    // byte _inputBuffer[1024];
    // int _inBufferCount;
	

	SI2C::SI2C() {

	};

	bool SI2C::sendByte(byte data) {
	/*!
     * @brief sends one byte as the controller.
     * @param data byte to be sent
	 * @return True if send fails(no ack is received)
     */	
		digitalWrite(_SCL, LOW);
		int halfDelay = (1000000/_frequency)/2;
		for(int i = 0; i < 8; i++) {
			digitalWrite(_SDA, data & (1 << (7 - i))); // set SDA to the i'th bit of the current byte, starting at the MSB
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, HIGH);
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW);
		}
		pinMode(_SDA, INPUT_PULLUP); // set to input to read ack.
		delayMicroseconds(halfDelay);
		digitalWrite(_SCL, HIGH);
		bool ret = digitalRead(_SDA); // returns true if message was not acknowledged
		pinMode(_SDA, OUTPUT); // set back to output
		return ret;
	}

	// bool SI2C::receiveByte() {
	// /*!
    //  * @brief reads one byte as the controller.
	//  * @return True if receive fails(buffer is full)
    //  */	
	// 	digitalWrite(_SCL, LOW);
	// 	int halfDelay = (1000000/_frequency)/2;
	// 	byte receivedByte = 0;
	// 	for(unsigned int i = 0; i < 8; i++) {
	// 		delayMicroseconds(halfDelay);
	// 		digitalWrite(_SCL, HIGH);
	// 		receivedByte += digitalRead(_SDA);
	// 		receivedByte << 1;
	// 		delayMicroseconds(halfDelay);
	// 		digitalWrite(_SCL, LOW);
	// 	}
	// 	pinMode(_SDA, OUTPUT); // set to output to transmit ack.
	// 	if(inBufferCount <= 31) {
	// 		_inputBuffer[inBufferCount] = receivedByte;
	// 		inBufferCount += 1;
	// 		digitalWrite(_SDA, LOW); // transmit ack
	// 	}
	// 	else {
	// 		digitalWrite(_SDA, HIGH); // transmit nack, ending transmission as buffer is full
	// 		delayMicroseconds(halfDelay);
	// 		digitalWrite(_SCL, HIGH);
	// 		return true;
	// 	}
	// 	delayMicroseconds(halfDelay);
	// 	digitalWrite(_SCL, HIGH);
	// 	return false;
	// }

    // public:
    void SI2C::begin(int SDA, int SCL, int freq) {
    /*!
     * @brief begins the I2C connection
     * @param SDA pin for data line
     * @param SCL pin for clock line
     * @param freq clock frequency of connection
     */
        SI2C::_SDA = SDA;
        SI2C::_SCL = SCL;
        SI2C::_frequency = freq;
        inBufferCount = 0;
		
		pinMode(SI2C::_SCL, OUTPUT);
    }
    // void SI2C::end();
    /*!
     * @brief closes the I2C connection
     */
    bool SI2C::writeTo(int dest, byte* data, size_t size) {
    /*!
     * @brief writes data to destination over I2C
     * @param dest device ID of destination
     * @param data data to be transferred
     * @param size number of bytes to be transferred
     * @return if write was sucessful
     */
		int clockDelay = 1000000/_frequency; //length of full clock cycle, in microseconds
		int halfDelay = clockDelay/2;  //length of half clock cycle, in microseconds
		pinMode(_SDA, OUTPUT);
		pinMode(_SCL, OUTPUT);
        byte startByte = dest << 1; // LSB is 0(Write)
		bool sent = false;
		byte tries = 0;
		while(sent == false && tries < 5) {
			digitalWrite(_SDA, HIGH);
			digitalWrite(_SCL, HIGH);

			digitalWrite(_SDA, LOW); //start condition
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW); //needs to be low for write
			delayMicroseconds(clockDelay);
			if(sendByte(startByte)) {
				tries++;
				continue;
			}
			sent = true;
			delayMicroseconds(halfDelay);
			for(unsigned int i = 0; i < size; i++) {
				digitalWrite(_SCL, LOW);
				byte curByte = data[i];
				// send the current byte, if it fails, exit loop and mark as failed
				if(sendByte(curByte)) {
					sent = false;
					tries++;
					break;
				}

			}
			delayMicroseconds(halfDelay);
			//set both to low to prepare for STOP signal
			digitalWrite(_SCL, 0);
			digitalWrite(_SDA, 0);

			//send STOP signal 
			digitalWrite(_SCL, 1);
			digitalWrite(_SDA, 1);

		}
		return true;
    }
    void SI2C::readFrom(int dest, size_t size) {
    /*!
     * @brief requests data from destination device, stores it in buffer
     * @param dest device ID of source
     * @param size number of bytes to be transferred
     */
		int clockDelay = 1000000/_frequency; //length of full clock cycle, in microseconds
		int halfDelay = clockDelay/2;  //length of half clock cycle, in microseconds
		pinMode(_SDA, OUTPUT);
        byte startByte = (dest << 1) + 1; // LSB is 0(Write)
		bool received = false;
		while(received == false) {
			digitalWrite(_SDA, HIGH);
			digitalWrite(_SCL, HIGH);

			digitalWrite(_SDA, LOW); //start condition
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW); //needs to be low for write
			delayMicroseconds(clockDelay);
			
			//send out initial byte with target address and mode. If it fails, try again
			if(sendByte(startByte)) {
				continue;
			}
			received = true;
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW);
			for(unsigned int i = 0; i < size; i++) {
				byte curByte = 0;
				for(int j = 0; j < 8; j++) {
					delayMicroseconds(halfDelay);
					digitalWrite(_SCL, HIGH);
					curByte += digitalRead(_SDA);
					curByte << 1;
					delayMicroseconds(halfDelay);
					digitalWrite(_SCL, LOW);
				}
				pinMode(_SDA, OUTPUT); // set to input to transmit ack.
				if(inBufferCount <= 31) {
					_inputBuffer[inBufferCount] = curByte;
					inBufferCount += 1;
					digitalWrite(_SCL, LOW); // transmit ack
				}
				else {
					digitalWrite(_SCL, HIGH); // transmit nack, ending transmission as buffer is full
					break;
				}
				delayMicroseconds(halfDelay);
				digitalWrite(_SCL, HIGH);
			}
			delayMicroseconds(halfDelay);
			//set both to low to prepare for STOP signal
			digitalWrite(_SCL, LOW);
			digitalWrite(_SDA, LOW);

			//send STOP signal 
			digitalWrite(_SCL, HIGH);
			digitalWrite(_SDA, HIGH);

		}
    }
    byte SI2C::readBuffer() {
     /*!
     * @brief requests a byte of data from the input buffer
     * @return most recently received byte from buffer that has not been returned yet
     */
		inBufferCount -= 1;
		return _inputBuffer[inBufferCount];

    }


	SI2C I2C = SI2C();