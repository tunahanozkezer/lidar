/*
 * uart_wrapper.cpp
 *
 *  Created on: Jan 5, 2024
 *      Author: tunah
 */


#include <uart_wrapper.hpp>

uart_wrapper::uart_wrapper(std::shared_ptr<UART_HandleTypeDef> huart) :  huart_{huart}, mutex{xSemaphoreCreateMutex()}
{
}
void uart_wrapper::sendData(const std::unique_ptr<uint8_t[]> &p_data, uint16_t size) {

	if( mutex != NULL )
	{
		xSemaphoreTake(mutex,portMAX_DELAY);
		HAL_UART_Transmit_DMA(huart_.get(), p_data.get(), size);
		xSemaphoreGive(mutex);

	}

}
