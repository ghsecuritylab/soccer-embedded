/**
  *****************************************************************************
  * @file    UartDriver.cpp
  * @author  Tyler Gamvrelis
  *
  * @defgroup UartDriver
  * @ingroup UART
  * @brief Manages the usage of a particular UART, taking care of
  *        hardware-level calls, OS-level calls, and passing up statuses
  * @{
  *****************************************************************************
  */




/********************************* Includes **********************************/
#include "UartDriver.h"

#if defined(THREADED)
#include "Notification.h"
#endif




namespace uart{
/********************************* UartDriver ********************************/
// Public
// ----------------------------------------------------------------------------
UartDriver::UartDriver(){
    m_max_block_time = 2;
}

#if defined(THREADED)
UartDriver::UartDriver(
    OsInterface* os_if,
    UartInterface* hw_if,
    UART_HandleTypeDef* uartHandlePtr
) :
    os_if(os_if),
    hw_if(hw_if),
    uartHandlePtr(uartHandlePtr)
{
    if(hw_if != nullptr && uartHandlePtr != nullptr){
        hw_is_initialized = true;
    }

    m_max_block_time = pdMS_TO_TICKS(2);
}
#else
UartDriver::UartDriver(
    UartInterface* hw_if,
    UART_HandleTypeDef* uartHandlePtr
) :
    hw_if(hw_if),
    uartHandlePtr(uartHandlePtr)

{
    if(hw_if != nullptr && uartHandlePtr != nullptr){
        hw_is_initialized = true;
    }
}
#endif

void UartDriver::setMaxBlockTime(uint32_t timeout){
    m_max_block_time = timeout;
}

void UartDriver::setIOType(IO_Type io_type){
    this->io_type = io_type;
}

IO_Type UartDriver::getIOType(void) const{
    return this->io_type;
}

bool UartDriver::transmit(
    uint8_t* arrTransmit,
    size_t numBytes
) const
{
#if defined(THREADED)
    uint32_t notification = 0;
    BaseType_t status = pdFALSE;
#endif
    HAL_StatusTypeDef hal_status;
    bool retval = false;

    if(hw_is_initialized){
        switch(io_type) {
#if defined(THREADED)
            case IO_Type::DMA:
                if(os_if != nullptr){
                    if(hw_if->transmitDMA(uartHandlePtr, arrTransmit, numBytes) == HAL_OK){
                        status = os_if->OS_xTaskNotifyWait(0, NOTIFIED_FROM_TX_ISR, &notification, m_max_block_time);

                        if((status == pdTRUE) && CHECK_NOTIFICATION(notification, NOTIFIED_FROM_TX_ISR)){
                            retval = true;
                        }
                    }
                }
                break;
            case IO_Type::IT:
                if(os_if != nullptr){
                    if(hw_if->transmitIT(uartHandlePtr, arrTransmit, numBytes) == HAL_OK){
                        status = os_if->OS_xTaskNotifyWait(
                            0,
                            NOTIFIED_FROM_TX_ISR,
                            &notification,
                            m_max_block_time
                        );

                        if((status == pdTRUE) &&
                           CHECK_NOTIFICATION(notification, NOTIFIED_FROM_TX_ISR))
                        {
                            retval = true;
                        }
                    }
                }
                break;
#endif
            case IO_Type::POLL:
            default:
                hal_status = hw_if->transmitPoll(
                    uartHandlePtr,
                    arrTransmit,
                    numBytes,
                    m_max_block_time
                );

                retval = (hal_status == HAL_OK);
                break;
        }
#if defined(THREADED)
        if(retval != true){
            hw_if->abortTransmit(uartHandlePtr);
        }
#endif
    }

    return retval;
}

bool UartDriver::receive(
    uint8_t* arrReceive,
    size_t numBytes
) const
{
#if defined(THREADED)
    uint32_t notification = 0;
    BaseType_t status = pdFALSE;
#endif
    HAL_StatusTypeDef hal_status;
    bool retval = false;

    if(hw_is_initialized){
        switch(io_type) {
#if defined(THREADED)
            case IO_Type::DMA:
                if(os_if != nullptr){
                    if(hw_if->receiveDMA(uartHandlePtr, arrReceive, numBytes) == HAL_OK){
                        status = os_if->OS_xTaskNotifyWait(0, NOTIFIED_FROM_RX_ISR, &notification, m_max_block_time);

                        if((status == pdTRUE) && CHECK_NOTIFICATION(notification, NOTIFIED_FROM_RX_ISR)){
                            retval = true;
                        }
                    }
                }
                break;
            case IO_Type::IT:
                if(os_if != nullptr){
                    if(hw_if->receiveIT(uartHandlePtr, arrReceive, numBytes) == HAL_OK){
                        status = os_if->OS_xTaskNotifyWait(
                            0,
                            NOTIFIED_FROM_RX_ISR,
                            &notification,
                            m_max_block_time
                        );

                        if((status == pdTRUE) &&
                           CHECK_NOTIFICATION(notification, NOTIFIED_FROM_RX_ISR))
                        {
                            retval = true;
                        }
                    }
                }
                break;
#endif
            case IO_Type::POLL:
            default:
                hal_status = hw_if->receivePoll(
                    uartHandlePtr,
                    arrReceive,
                    numBytes,
                    m_max_block_time
                );

                retval = (hal_status == HAL_OK);
                break;
        }
#if defined(THREADED)
        if(retval != true){
            hw_if->abortReceive(uartHandlePtr);
        }
#endif
    }

    return retval;
}

} // end namespace uart




/**
 * @}
 */
/* end - UartDriver */
