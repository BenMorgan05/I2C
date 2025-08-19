	#include "I2C.h"


    // int _SDA;
    // int _SCL;
    // int _frequency;
    // byte _inputBuffer[1024];
    // int _inBufferCount;


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

		pinMode(I2C::_SDA, 1);
        byte startByte = dest << 1; // LSB is 0(Write)
		bool sent = false;
		while(sent == false) {
			digitalWrite(I2C::_SDA, 1);
			digitalWrite(I2C::_SCL, 1);
			delay(1000000/_frequency);

			digitalWrite(I2C::_SDA, 0); //start condition
			delayMicroseconds(1000000/_frequency);
			digitalWrite(I2C::_SCL, 0); //needs to be low for write

			tone(I2C::_SCL, _frequency * 1000);
			shiftOut(I2C::_SDA, I2C::_SCL, MSBFIRST, startByte); // shift out initial byte containing target address and access type

			if(!digitalRead(I2C::_SDA)) {
				continue;
				// if start message fails, restart loop
			}
			sent = true;
			for(int i = 0; i < size; i++) {
				shiftOut(I2C::_SDA, I2C::_SCL, MSBFIRST, data[i]); // shift out current byte
				delayMicroseconds(1000000/_frequency); // delay one clock cycle
				if(!digitalRead(I2C::_SDA)) {
					// if not acknowledged, restart the transfer
					sent = false;
					break;
				}
			}
			noTone(I2C::_SCL);

			//set both to low to prepare for STOP signal
			digitalWrite(I2C::_SCL, 0);
			digitalWrite(I2C::_SDA, 0);

			//send STOP signal 
			digitalWrite(I2C::_SCL, 1);
			digitalWrite(I2C::_SDA, 1);

		}
    }
    void I2C::readFrom(int dest, size_t size) {
    /*!
     * @brief requests data from destination device, stores it in buffer
     * @param dest device ID of source
     * @param size number of bytes to be transferred
     */
		
    }
    byte I2C::readBuffer() {
     /*!
     * @brief requests a byte of data from the input buffer
     * @return byte read from buffer
     */
    }
