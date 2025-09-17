#ifndef I2C_H
#define I2C_H
#include "arduino.h"
class SI2C {
    private:
    int _SDA;
    int _SCL;
    int _frequency;
    byte _inputBuffer[32];
    int inBufferCount;

	bool sendByte(byte data);
	bool receiveByte();


    public:
    SI2C();
	/*!
     * @brief begins the I2C connection
     * @param SDA pin for data line
     * @param SCL pin for clock line
     * @param freq clock frequency of connection
     */
    void begin(int SDA, int SCL, int freq);
    /*!
     * @brief closes the I2C connection
     */
    void end();
    /*!
     * @brief writes data to destination over I2C
     * @param dest device ID of destination
     * @param data data to be transferred
     * @param size number of bytes to be transferred
     * @return if write was sucessful
     */
    bool writeTo(int dest, byte* data, size_t size);
    /*!
     * @brief requests data from destination device, stores it in buffer to be retrieved with readBuffer()
     * @param dest device ID of source
     * @param size number of bytes to be read
     */
    void readFrom(int dest, size_t size);
     /*!
     * @brief requests a byte of data from the input buffer
     * @return most recently received byte from buffer that has not been returned yet
     */
    byte readBuffer();
};


extern SI2C I2C;



#endif