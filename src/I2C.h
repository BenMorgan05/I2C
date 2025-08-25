#ifndef I2C_H
#define I2C_H
#include "arduino.h"
class I2C {
    private:
    int _SDA;
    int _SCL;
    int _frequency;
    byte _inputBuffer[32];
    int inBufferCount;


    public:
    I2C();
    void begin(int SDA, int SCL, int freq);
    /*!
     * @brief begins the I2C connection
     * @param SDA pin for data line
     * @param SCL pin for clock line
     * @param freq clock frequency of connection
     */
    void end();
    /*!
     * @brief closes the I2C connection
     */
    bool writeTo(int dest, byte* data, size_t size);
    /*!
     * @brief writes data to destination over I2C
     * @param dest device ID of destination
     * @param data data to be transferred
     * @param size number of bytes to be transferred
     */
    void readFrom(int dest, size_t size);
    /*!
     * @brief requests data from destination device
     * @param dest device ID of source
     * @param size number of bytes to be transferred
     */
    byte readBuffer();
     /*!
     * @brief requests a byte of data from the input buffer
     * @return byte read from buffer
     */
};






#endif