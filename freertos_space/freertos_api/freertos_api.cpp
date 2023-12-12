/*
 * freertos_api.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */


#include "freertos_api.hpp"

rtos_task::rtos_task(void (*rtos_task)( void * pvParameters ), const char *text_name, UBaseType_t priority, configSTACK_DEPTH_TYPE task_heap)
{
	xTaskCreate(
			rtos_task,       /* Function that implements the task. */
			text_name,          /* Text name for the task. */
			task_heap,      /* Stack size in words, not bytes. */
			( void * ) 1,    /* Parameter passed into the task. */
			priority,/* Priority at which the task is created. */
			&handle );      /* Used to pass out the created task's handle. */

	task = rtos_task;
	heap = task_heap;
}
