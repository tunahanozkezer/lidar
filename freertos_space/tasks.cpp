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
#include <a4988.hpp>
#include <cstring>

extern uint32_t wave_count;
tf_luna luna_ct;

rtos_task comm_task_init(tasks::sensor_task  , "COMM_Task"    );
rtos_task hmi_task      (tasks::hmi_comm_task, "hmi_comm_task");
rtos_task lidar_task    (tasks::lidar_task   , "lidar_task",2 );

uint32_t pres{100000};
bool start_flg{false};
float angle_of_motor{};

void tasks::lidar_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 1;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	A4988_Step_Motor a4988(&htim10, TIM_CHANNEL_1, &wave_count);

	for(;;)
	{
		angle_of_motor = a4988.degree_state(pres, start_flg);


		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}


void tasks::sensor_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 10;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	dma_ring_buffer uart_1_buffer(&huart1, 20);

	luna_ct.output_control(output_en_state::ENABLE);
	for(;;)
	{
		std::vector<uint8_t> data_get = uart_1_buffer.veri_al();
		 luna_ct.parse_byte(data_get);

		HAL_GPIO_TogglePin(ob_led_GPIO_Port, ob_led_Pin);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

struct data_to_send
{
	uint16_t angle;
    uint16_t distance;
};


void tasks::hmi_comm_task( void * pvParameters)
{
	 constexpr TickType_t xFrequency = 50;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	dma_ring_buffer uart_2_buffer(&huart2, 20);
	for(;;)
	{
		data_to_send my_data;
		my_data.angle    = static_cast<uint16_t>(angle_of_motor*100);
		my_data.distance = luna_ct.distance_cm_u16;

		uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(&my_data);

		uart_protocol::packet packet_to_get;

		std::vector<uint8_t> data_get = uart_2_buffer.veri_al();

		if(uart_protocol::unpack_packet(data_get, packet_to_get))
		{
			uint8_t a{};
			std::memcpy(&a, packet_to_get.payload.data(), packet_to_get.packet_size);
			start_flg = a ==1;
		}

		std::vector<uint8_t> dataToSend{byte_ptr, byte_ptr + sizeof(data_to_send)};

		uint32_t packet_size;
		std::unique_ptr<uint8_t[]> packedData = uart_protocol::pack_packet(uart_protocol::data_packet, dataToSend, packet_size);

		// Gönderilecek veriyi ekrana yazdır
		HAL_UART_Transmit_DMA(&huart2, packedData.get(), packet_size);

		 vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
