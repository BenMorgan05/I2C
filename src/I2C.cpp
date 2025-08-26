	#include "I2C.h"


    // int _SDA;
    // int _SCL;
    // int _frequency;
    // byte _inputBuffer[1024];
    // int _inBufferCount;
	bool I2C::sendByte(byte data) {
	/*!
     * @brief begins the I2C connection
     * @param SDA pin for data line
     * @param SCL pin for clock line
     * @param freq clock frequency of connection
     */
		int halfDelay = (1000000/_frequency)/2;
		for(int i = 0; i < 8; i++) {
			digitalWrite(_SDA, data & (1 >> 7 - i)); // set SDA to the i'th bit of the current byte, starting at the MSB
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, HIGH);
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW);
		}
		pinMode(_SDA, INPUT_PULLUP); // set to input to read ack.
		delayMicroseconds(halfDelay);
		digitalWrite(_SCL, HIGH);
		return digitalRead(_SDA); // returns true if message was not acknowledged
	}

    // public:
    void I2C::begin(int SDA, int SCL, int freq) {
    /*!
     * @brief begins the I2C connection
     * @param SDA pin for data line
     * @param SCL pin for clock line
     * @param freq clock frequency of connection
     */
        I2C::_SDA = SDA;
        I2C::_SCL = SCL;
        I2C::_frequency = freq;
        inBufferCount = 0;
		
		pinMode(I2C::_SCL, 1);

        
    }
    // void I2C::end();
    /*!
     * @brief closes the I2C connection
     */
    bool I2C::writeTo(int dest, byte* data, size_t size) {
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
        byte startByte = dest << 1; // LSB is 0(Write)
		bool sent = false;
		while(sent == false) {
			digitalWrite(_SDA, HIGH);
			digitalWrite(_SCL, HIGH);

			digitalWrite(_SDA, LOW); //start condition
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW); //needs to be low for write
			delayMicroseconds(clockDelay);
			if(sendByte(startByte)) {
				continue;
			}
			sent = true;
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW);
			for(int i = 0; i < size; i++) {
				byte curByte = data[i];
				// send the current byte, if it fails, exit loop and mark as failed
				if(sendByte(curByte)) {
					sent = false;
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
    }
    void I2C::readFrom(int dest, size_t size) {
    /*!
     * @brief requests data from destination device, stores it in buffer
     * @param dest device ID of source
     * @param size number of bytes to be transferred
     */
		int clockDelay = 1000000/_frequency; //length of full clock cycle, in microseconds
		int halfDelay = clockDelay/2;  //length of half clock cycle, in microseconds
		pinMode(_SDA, OUTPUT);
        byte startByte = (dest << 1) + 1; // LSB is 0(Write)
		bool sent = false;
		while(sent == false) {
			digitalWrite(_SDA, HIGH);
			digitalWrite(_SCL, HIGH);

			digitalWrite(_SDA, LOW); //start condition
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW); //needs to be low for write
			delayMicroseconds(clockDelay);
			// shiftOut(I2C::_SDA, I2C::_SCL, MSBFIRST, startByte); // shift out initial byte containing target address and access type
			for(int i = 0; i < 8; i++) {
				digitalWrite(_SDA, startByte & (1 >> 7 - i)); // set SDA to the i'th bit of startByte, starting at the MSB
				delayMicroseconds(halfDelay);
				digitalWrite(_SCL, HIGH);
				delayMicroseconds(halfDelay);
				digitalWrite(_SCL, LOW);
			}
			pinMode(_SDA, INPUT_PULLUP); // set to input to read ack.
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, HIGH);
			if(digitalRead(_SDA)) {
				continue;
				// if start message fails, restart loop
			}
			sent = true;
			delayMicroseconds(halfDelay);
			digitalWrite(_SCL, LOW);
			for(int i = 0; i < size; i++) {
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
			digitalWrite(_SCL, 0);
			digitalWrite(_SDA, 0);

			//send STOP signal 
			digitalWrite(_SCL, 1);
			digitalWrite(_SDA, 1);

		}
    }
    byte I2C::readBuffer() {
     /*!
     * @brief requests a byte of data from the input buffer
     * @return most recently recieved byte from buffer that has not been returned yet
     */
		inBufferCount -= 1;
		return _inputBuffer[inBufferCount];

    }
