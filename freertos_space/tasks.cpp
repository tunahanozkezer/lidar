/*
 * tasks.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */


#include <tasks.hpp>
#include <usart.h>
#include <gpio.h>
#include <dma_ring_buffer.hpp>
#include <tf_luna.hpp>
#include <task.h>
#include <hmi_packets.hpp>

dma_ring_buffer uart_1_buffer(&huart1, 20);
dma_ring_buffer uart_2_buffer(&huart2, 20);
uint8_t cikilan_veri_u8[20];

tf_luna luna_ct(5);

rtos_task comm_task_init(tasks::comm_task, "COMM_Task");
rtos_task hmi_task(tasks::hmi_comm_task, "hmi_comm_task");

hmi_interface::packet packet_to_send;

void tasks::comm_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 10;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	 luna_ct.output_control(output_en_state::ENABLE);

	for(;;)
	{
		 uart_2_buffer.veri_al();
		 luna_ct.parse_byte(uart_2_buffer.data_qu);

		HAL_GPIO_TogglePin(ob_led_GPIO_Port, ob_led_Pin);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void tasks::hmi_comm_task( void * pvParameters)
{
	 constexpr TickType_t xFrequency = 1000;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		 uart_1_buffer.veri_al();

//		 hmi_packets dene;
//		 dene.send_periodic_packet();

		 uint8_t a [] = "Emotional";

		 uint8_t* b = packet_to_send.get_packed(a, sizeof(a), hmi_interface::types::PERIODIC);

		 HAL_UART_Transmit_DMA(&huart1, b, sizeof(a) - 1 +  packet_to_send.interface_size);

		 vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
