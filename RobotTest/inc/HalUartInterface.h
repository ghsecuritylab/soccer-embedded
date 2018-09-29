/**
  *****************************************************************************
  * @file    HalUartInterface.h
  * @author  Tyler Gamvrelis
  *
  * @defgroup   HalUartInterface
  * @addtogroup UART
  * @{
  *****************************************************************************
  */




#ifndef HAL_UART_INTERFACE_H
#define HAL_UART_INTERFACE_H




/********************************* Includes **********************************/
#include <cstdint>

#define STM32F446xx
#include <stm32f446xx.h>
#include "usart.h"
#include "UartInterface.h"




/****************************** HalUartInterface *****************************/
namespace UART{
// Classes and structs
// ----------------------------------------------------------------------------
/**
 * @class Concrete implementation of the abstract UartInterface class, to be
 *        used in production builds
 */
class HalUartInterface : UartInterface{
public:
    HalUartInterface();
    ~HalUartInterface() {}

    HAL_StatusTypeDef transmitPoll(
        uint8_t* arrTransmit,
        size_t numBytes,
        uint32_t timeout
    ) override final;

    HAL_StatusTypeDef receivePoll(
        uint8_t* arrReceive,
        size_t numBytes,
        uint32_t timeout
    ) override final;

#ifdef THREADED
    HAL_StatusTypeDef transmitIT(uint8_t* arrTransmit, size_t numBytes) override final;
    HAL_StatusTypeDef receiveIT(uint8_t* arrReceive, size_t numBytes) override final;
    HAL_StatusTypeDef transmitDMA(uint8_t* arrTransmit, size_t numBytes) override final;
    HAL_StatusTypeDef receiveDMA(uint8_t* arrReceive, size_t numBytes) override final;

    void abortTransmit() override final;
    void abortReceive() override final;
#endif

private:
    /**
     * @brief Address of the container for the UART module associated with this
     *        object
     */
    UART_HandleTypeDef* uartHandlePtr = nullptr;
};

} // end namespace UART




/**
 * @}
 */
/* end - Header */

#endif /* HAL_UART_INTERFACE_H */