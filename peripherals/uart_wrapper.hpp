/*
 * uart_wrapper.hpp
 *
 *  Created on: Jan 5, 2024
 *      Author: tunah
 */

#ifndef UART_WRAPPER_HPP_
#define UART_WRAPPER_HPP_

#include <usart.h>
#include <memory>
#include <FreeRTOS.h>
#include <semphr.h>

class uart_wrapper {
public:
	uart_wrapper(std::shared_ptr<UART_HandleTypeDef> huart);
    void sendData(const std::unique_ptr<uint8_t[]> &p_data, uint16_t size);

private:
    std::shared_ptr<UART_HandleTypeDef> huart_;
    SemaphoreHandle_t mutex;
};


#endif /* UART_WRAPPER_HPP_ */
