/*
 * tasks.hpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */

#ifndef TASKS_HPP_
#define TASKS_HPP_

#include "freertos_api/freertos_api.hpp"

class tasks
{
public:
	static void init_tasks();
	static void sensor_task( void * pvParameters );
	static void hmi_comm_task( void * pvParameters );
	static void lidar_task( void * pvParameters );
};



#endif /* TASKS_HPP_ */
