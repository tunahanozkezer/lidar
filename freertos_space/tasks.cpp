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

tf_luna luna_ct;

rtos_task comm_task_init(tasks::comm_task, "COMM_Task");
rtos_task hmi_task(tasks::hmi_comm_task, "hmi_comm_task");

void tasks::comm_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 100;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	 luna_ct.output_control(output_en_state::ENABLE);

	for(;;)
	{
		std::vector<uint8_t> data_get = uart_1_buffer.veri_al();
		 luna_ct.parse_byte(data_get);

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
		uart_protocol::packet packet_to_get;

		std::vector<uint8_t> data_get = uart_2_buffer.veri_al();

//		uart_protocol::unpack_packet(data_get, packet_to_get);

		std::vector<uint8_t> dataToSend = {static_cast<uint8_t>((0xFF)&luna_ct.distance_cm_u16>>8), static_cast<uint8_t>((0xFF)&luna_ct.distance_cm_u16),
				                           static_cast<uint8_t>((0xFF)&luna_ct.amp_u16>>8)        , static_cast<uint8_t>((0xFF)&luna_ct.amp_u16)};

		uint32_t packet_size;
		std::unique_ptr<uint8_t[]> packedData = uart_protocol::pack_packet(uart_protocol::data_packet, dataToSend, packet_size);

		// Gönderilecek veriyi ekrana yazdır
		HAL_UART_Transmit_DMA(&huart2, packedData.get(), packet_size);

		 vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
