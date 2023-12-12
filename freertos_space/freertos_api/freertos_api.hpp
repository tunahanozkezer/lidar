/*
 * freertos_api.hpp
 *
 *  Created on: Nov 3, 2023
 *      Author: tunah
 */

#ifndef FREERTOS_API_HPP_
#define FREERTOS_API_HPP_

#include "FreeRTOS.h"
#include "task.h"

#define MIN_HEAP_SIZE 128
class rtos_task
{

	public:
	void (*task)( void * pvParameters );
	rtos_task(void (*rtos_task)( void * pvParameters ), const char *text_name, UBaseType_t priority = 1, configSTACK_DEPTH_TYPE task_heap  = MIN_HEAP_SIZE);
	private:
	configSTACK_DEPTH_TYPE heap;
	TaskHandle_t handle;

};

#endif /* FREERTOS_API_HPP_ */
