/*
 * freertos_api.hpp
 *
 *  Created on: Nov 3, 2023
 *      Author: tunah
 */

#ifndef FREERTOS_API_HPP_
#define FREERTOS_API_HPP_

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

class rtos_task
{
	public:
	rtos_task(void (*rtos_task)( void * pvParameters ), const char *text_name, UBaseType_t priority = 1, configSTACK_DEPTH_TYPE task_heap  = min_heap_size) : task{rtos_task}, heap{task_heap}
	{
		if(rtos_task != NULL || text_name != NULL)
		{
			xTaskCreate(
					rtos_task   ,  /* Function that implements the task. */
					text_name   ,  /* Text name for the task. */
					task_heap   ,  /* Stack size in words, not bytes. */
					( void * ) 1,  /* Parameter passed into the task. */
					priority    ,  /* Priority at which the task is created. */
					&handle     ); /* Used to pass out the created task's handle. */
		}
	}

private:
	static constexpr auto min_heap_size = 128;
	void (*task)( void * pvParameters );
	configSTACK_DEPTH_TYPE heap;
	TaskHandle_t handle;

};

class rtos_ui
{
	public:


	rtos_ui() :pwm_freq_mutex{xSemaphoreCreateMutex()},
			start_flag_mutex{xSemaphoreCreateMutex()},
			angle_of_motor_mutex{xSemaphoreCreateMutex()},
			motor_state_mutex{xSemaphoreCreateMutex()},
			sensor_distance_mutex{xSemaphoreCreateMutex()},
			dummy{xSemaphoreCreateMutex()}
			{}

    // Verilere güvenli erişim sağlamak için kilitleme (lock) fonksiyonu
    void lock(SemaphoreHandle_t mutex_to_lock) {
    	xSemaphoreTake(mutex_to_lock, portMAX_DELAY);
    }

    // Verilere güvenli erişim sağlamak için kilidi (lock) bırakma fonksiyonu
    void unlock(SemaphoreHandle_t mutex_to_lock) {
    	xSemaphoreGive(mutex_to_lock);
    }

    void set_pwm_freq(uint32_t tmp_pwm_freq) {
        lock(pwm_freq_mutex);
        pwm_freq = tmp_pwm_freq;
        unlock(pwm_freq_mutex);
    }

    void set_start_flag(bool tmp_start_flag) {
        lock(start_flag_mutex);
        start_flag = tmp_start_flag;
        unlock(start_flag_mutex);
    }

    void set_angle_of_motor(float tmp_angle_of_motor) {
        lock(angle_of_motor_mutex);
        angle_of_motor = tmp_angle_of_motor;
        unlock(angle_of_motor_mutex);
    }

    void set_motor_state(uint8_t tmp_motor_state) {
        lock(motor_state_mutex);
        motor_state = tmp_motor_state;
        unlock(motor_state_mutex);
    }

    void set_sensor_distance(uint16_t tmp_sensor_distance_cm) {
        lock(sensor_distance_mutex);
        sensor_distance_cm = tmp_sensor_distance_cm;
        unlock(sensor_distance_mutex);
    }


    uint32_t get_pwm_freq() {
        lock(pwm_freq_mutex);
        uint32_t tmp_pwm_freq = pwm_freq;
        unlock(pwm_freq_mutex);
        return tmp_pwm_freq;
    }

    bool get_start_flag() {
        lock(start_flag_mutex);
        bool tmp_start_flag = start_flag;
        unlock(start_flag_mutex);
        return tmp_start_flag;
    }

    float get_angle_of_motor() {
        lock(angle_of_motor_mutex);
        float tmp_angle_of_motor = angle_of_motor;
        unlock(angle_of_motor_mutex);
        return tmp_angle_of_motor;
    }

    uint8_t get_motor_state() {
        lock(motor_state_mutex);
        uint8_t tmp_motor_state = motor_state;
        unlock(motor_state_mutex);
        return tmp_motor_state;
    }

    uint16_t get_sensor_distance_cm() {
        lock(sensor_distance_mutex);
        uint16_t tmp_sensor_distance_cm = sensor_distance_cm;
        unlock(sensor_distance_mutex);
        return tmp_sensor_distance_cm;
    }


private:
    SemaphoreHandle_t pwm_freq_mutex       {};
    SemaphoreHandle_t start_flag_mutex     {};
    SemaphoreHandle_t angle_of_motor_mutex {};
    SemaphoreHandle_t motor_state_mutex    {};
    SemaphoreHandle_t sensor_distance_mutex{};
    SemaphoreHandle_t dummy    {};

    uint32_t pwm_freq{200000};
    bool start_flag{false};
    float angle_of_motor{};
    uint8_t motor_state{};
    uint16_t sensor_distance_cm{};

};
#endif /* FREERTOS_API_HPP_ */
