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

uint32_t pres{200000};
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

	dma_ring_buffer tf_luna_buffer(&huart1, 50);

	luna_ct.output_control(output_en_state::ENABLE);
	for(;;)
	{
		static auto no_data = 0U;
		std::vector<uint8_t> data_get = tf_luna_buffer.get_data();

		if(data_get.size() == 0)
		{
			no_data++;
			if(no_data > 20)
			{
				luna_ct.output_control(output_en_state::ENABLE);
				no_data = 0;
			}
		}
		 luna_ct.parse_byte(data_get);
		HAL_GPIO_TogglePin(ob_led_GPIO_Port, ob_led_Pin);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void tasks::hmi_comm_task( void * pvParameters)
{
	 constexpr TickType_t xFrequency = 10;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	dma_ring_buffer hmi_buffer(&huart2, 50);

	for(;;)
	{
		std::vector<uint8_t> data_get = hmi_buffer.get_data();

		uart_protocol::packet packet_to_get;

		if(uart_protocol::unpack_packet(data_get, packet_to_get))
		{
			hmi_packets::packet_parse(packet_to_get);
			data_get.clear();
		}

		uint32_t packet_size{};
		const uart_protocol::packet packet_to_send{ hmi_packets::packet_periodic(luna_ct.distance_cm_u16, angle_of_motor)};
		std::unique_ptr<uint8_t[]> data__to_send = uart_protocol::packet_to_ptr(packet_to_send, packet_size);

		HAL_UART_Transmit_DMA(&huart2, data__to_send.get(), packet_size);

		 vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
