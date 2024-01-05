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
#include <memory>

extern uint32_t wave_count;
tf_luna luna_ct;

rtos_task comm_task_init(tasks::sensor_task  , "COMM_Task"        );
rtos_task hmi_task      (tasks::hmi_comm_task, "hmi_comm_task"    );
rtos_task lidar_task    (tasks::lidar_task   , "lidar_task"    ,2 );

uint32_t pwm_freq{200000};
bool start_flg{false};
float angle_of_motor{};
motor_state motor_state{};

void tasks::lidar_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 1;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	A4988_Step_Motor a4988(std::shared_ptr<TIM_HandleTypeDef>(&htim10), TIM_CHANNEL_1, std::shared_ptr<uint32_t>(&wave_count));

	for(;;)
	{
		angle_of_motor = a4988.degree_state(pwm_freq, start_flg, motor_state);


		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}


void tasks::sensor_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 10;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	dma_ring_buffer tf_luna_buffer(&huart1, 50);

	for(;;)
	{

		std::vector<uint8_t> data_get = tf_luna_buffer.get_data();
		luna_ct.parse_byte(data_get);
		luna_ct.output_control(output_en_state::ENABLE);

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

void tasks::hmi_comm_task( void * pvParameters)
{
	constexpr TickType_t xFrequency = 10;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	dma_ring_buffer hmi_buffer(&huart2, 50);

	uart_wrapper wrap((std::shared_ptr<UART_HandleTypeDef>(&huart2)));
	hmi_packets packettt(wrap);

	for(;;)
	{
		std::vector<uint8_t> data_get = hmi_buffer.get_data();

		hmi_packets::packet_parse(data_get);

		packettt.packet_periodic(luna_ct.distance_cm_u16, angle_of_motor, motor_state);

//		HAL_UART_Transmit_DMA(&huart2, data__to_send.get(), packet_size);

		 vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}
