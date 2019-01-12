/**
  ******************************************************************************
  * @file
  * @author Izaak
  * @author Jenny
  * @author Tyler
  *
  * @defgroup MPU6050
  * @brief Interfaces for reading data from a MPU6050 sensor, as well as
  *        setting control registers.
  * @{
  ******************************************************************************
  */




#ifndef MPU6050_H
#define MPU6050_H




/********************************* Includes **********************************/
#include <stdint.h>
#include "i2c.h"




/********************************* MPU6050 ***********************************/
namespace imu{
// Classes and structs
// ----------------------------------------------------------------------------
/**
 * @brief The data structure which represents the data of the IMU, which is
 *        sent in queues between tasks
 */
typedef struct{
    float x_Gyro;  /**< x-axis angular velocity read from sensor */
    float y_Gyro;  /**< y-axis angular velocity read from sensor */
    float z_Gyro;  /**< z-axis angular velocity read from sensor */
    float x_Accel; /**< x-axis acceleration read from sensor     */
    float y_Accel; /**< y-axis acceleration read from sensor     */
    float z_Accel; /**< z-axis acceleration read from sensor     */
}IMUStruct_t;


class MPU6050 {
public:
    /**
     * @brief The constructor for the MPU6050 class, which initializes non-I/O members
     * @param I2CHandle A pointer to the I2C_HandleTypeDef being used
     */
    MPU6050(I2C_HandleTypeDef* I2CHandle);

    /**
     * @brief This function is used to initialize all aspects of the IMU
     *        which require I/O
     */
    void init();

    /**
     * @brief Sets the offsets of the sensor. Note that a lower setting
     *        implies smaller bandwidth but higher signal delay
     * @param lpf The setting being used for the built-in DLPF. Must be
     *        less than or equal to 6 (6 -> 5 Hz bandwidth, 0 -> about 260
     *        Hz bandwidth)
     * @return true if successful, otherwise false
     */
    bool Set_LPF(uint8_t lpf);

    /**
     * @brief Reads the gyroscope with offsets without interrupts
     */
    void Read_Gyroscope();

    /**
     * @brief Reads the gyroscope with interrupts and offsets
     */
    void Read_Gyroscope_IT();

    /**
     * @brief Reads the accelerometer with offsets without interrupts
     */
    void Read_Accelerometer();

    /**
     * @brief Reads the accelerometer with interrupts and offsets
     */
    void Read_Accelerometer_IT();

    /**
     * @brief Fills an IMUStruct
     * @param myStruct The pointer to the struct being filled
     */
    void Fill_Struct(IMUStruct_t* myStruct);

    /**
     * @brief The MPU6050 desctructor
     */
    ~MPU6050() {}

private:
    /**
     * @brief Writes to a register from the MPU6050
     * @param reg_addr uint8_t address of the register
     * @param data uint8_t data to be written
     * @return Status
     */
    HAL_StatusTypeDef Write_Reg(uint8_t reg_addr, uint8_t data);

    /**
     * @brief Reads a byte from a register on the sensor, and stores it
     *        in the member variable received_byte
     * @param reg_addr uint8_t address of the register
     * @return Status
     */
    HAL_StatusTypeDef Read_Reg(uint8_t reg_addr);

    /**
     * @brief Reads 6 bytes from the sensor with interrupts, and stores them in
     *        the sensor_buffer
     * @param reg_addr uint8_t address of the register
     * @param sensor_buffer uint8_t pointer to output buffer
     * @return Status
     */
    HAL_StatusTypeDef Read_Data_IT(uint8_t Reg_addr, uint8_t* sensor_buffer);

    /**
     * @brief Reads 6 bytes from the sensor, and stores them in
     *        the sensor_buffer
     * @param reg_addr uint8_t address of the register
     * @param sensor_buffer uint8_t pointer to output buffer
     * @return Status
     */
    HAL_StatusTypeDef Read_Data(uint8_t Reg_addr, uint8_t* sensor_buffer);

    I2C_HandleTypeDef* I2C_Handle; /**< I2C handle associated with sensor instance */
    float              x_Gyro;     /**< x-axis angular velocity read from sensor */
    float              y_Gyro;     /**< y-axis angular velocity read from sensor */
    float              z_Gyro;     /**< z-axis angular velocity read from sensor */
    float              x_Accel;    /**< x-axis acceleration read from sensor */
    float              y_Accel;    /**< y-axis acceleration read from sensor */
    float              z_Accel;    /**< z-axis acceleration read from sensor */
    uint8_t            received_byte;
};

} // end namespace imu


/**
 * @}
 */
/* end - Header */

#endif /* MPU6050_H_ */
