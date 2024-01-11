/*
 * tasks.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */

#include "tasks.hpp"
#include "dma_ring_buffer.hpp"
#include "tf_luna.hpp"
#include "task.h"
#include "a4988.hpp"
#include "uart_wrapper.hpp"
#include "hmi_packets.hpp"

uint32_t wave_count{}; // stepper motor clock count. Just here for read. look at "TimerOverflowCallback"
rtos_ui os_ui;
rtos_task sensor_task (tasks::sensor_task  , "COMM_Task"        );
rtos_task hmi_task    (tasks::hmi_task     , "hmi_comm_task",2  );
rtos_task lidar_task  (tasks::lidar_task   , "lidar_task"   ,3  );

void tasks::lidar_task( void * pv_parameters)
{
	constexpr TickType_t task_frequency{1};
	TickType_t last_wake_time{xTaskGetTickCount()};
	A4988_Step_Motor a4988(std::shared_ptr<TIM_HandleTypeDef>(&htim10), TIM_CHANNEL_1);
	for(;;)
	{
		motor_states motor_state = static_cast<motor_states>(os_ui.get_motor_state());
		os_ui.set_angle_of_motor(a4988.degree_state( os_ui.get_pwm_freq(), os_ui.get_start_flag(), wave_count, motor_state));
		os_ui.set_motor_state(static_cast<uint8_t>(motor_state));
		vTaskDelayUntil( &last_wake_time, task_frequency );
	}
}

void tasks::sensor_task( void * pv_parameters)
{
	TickType_t task_frequency{10};
	TickType_t last_wake_time{xTaskGetTickCount()};

	dma_ring_buffer tf_luna_buffer(std::shared_ptr<UART_HandleTypeDef>(&huart1), 30);
	uart_wrapper uart_send_wrapper((std::shared_ptr<UART_HandleTypeDef>(&huart1)));
	tf_luna<uart_wrapper> luna_ct(uart_send_wrapper);

	for(;;)
	{
		luna_ct.parse_byte(tf_luna_buffer.get_data());
		os_ui.set_sensor_distance(luna_ct.distance_cm_u16);
		task_frequency = luna_ct.output_control();
		vTaskDelayUntil( &last_wake_time, task_frequency );
	}
}

void tasks::hmi_task( void * pv_parameters)
{
	constexpr TickType_t task_frequency{10};
	TickType_t last_wake_time{xTaskGetTickCount()};

	dma_ring_buffer hmi_buffer(std::shared_ptr<UART_HandleTypeDef>(&huart2), 20);
	uart_wrapper uart_send_wrapper((std::shared_ptr<UART_HandleTypeDef>(&huart2)));
	hmi_packets<uart_wrapper> hmi_comm(uart_send_wrapper);

	for(;;)
	{
		hmi_comm.packet_parse(hmi_buffer.get_data());
		hmi_comm.packet_periodic(os_ui.get_sensor_distance_cm(), os_ui.get_angle_of_motor(), static_cast<motor_states>(os_ui.get_motor_state()));
		vTaskDelayUntil( &last_wake_time, task_frequency );
	}
}
