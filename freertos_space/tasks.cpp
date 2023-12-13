/*
 * tasks.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */


#include "tasks.hpp"
#include "usart.h"
#include "gpio.h"
#include "dma_ring_buffer.hpp"
#include "tf_luna.hpp"
#include "task.h"

uint8_t tx_buff[]={0,1,2,3,4,5,6,7,8,9};


dma_ring_buffer uart_1_buffer(&huart1, 20);
dma_ring_buffer uart_2_buffer(&huart2, 20);
uint8_t cikilan_veri_u8[20];

tf_luna luna_ct(5);

rtos_task comm_task_init(tasks::comm_task, "COMM_Task");

void tasks::comm_task( void * pvParameters)
{
	 const TickType_t xFrequency = 10;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	 luna_ct.output_control(output_en_state::ENABLE);

	for(;;)
	{
		 uart_1_buffer.veri_al();
		 uart_2_buffer.veri_al();
		 luna_ct.parse_byte(uart_2_buffer.data_qu);

		HAL_GPIO_TogglePin(ob_led_GPIO_Port, ob_led_Pin);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
