/*
 * a4988.cpp
 *
 *  Created on: Dec 25, 2023
 *      Author: tunah
 */

#include "a4988.hpp"

#define ob_led_Pin GPIO_PIN_13
#define ob_led_GPIO_Port GPIOC
#define ob_btn_Pin GPIO_PIN_0
#define ob_btn_GPIO_Port GPIOA
#define a4988_en_Pin GPIO_PIN_15
#define a4988_en_GPIO_Port GPIOA
#define a4988_ms1_Pin GPIO_PIN_3
#define a4988_ms1_GPIO_Port GPIOB
#define a4988_ms2_Pin GPIO_PIN_4
#define a4988_ms2_GPIO_Port GPIOB
#define a4988_ms3_Pin GPIO_PIN_5
#define a4988_ms3_GPIO_Port GPIOB
#define a4988_rst_Pin GPIO_PIN_6
#define a4988_rst_GPIO_Port GPIOB
#define a4988_slp_Pin GPIO_PIN_7
#define a4988_slp_GPIO_Port GPIOB
#define a4988_stp_Pin GPIO_PIN_8
#define a4988_stp_GPIO_Port GPIOB
#define a4988_dir_Pin GPIO_PIN_9
#define a4988_dir_GPIO_Port GPIOB

uint32_t rotation = 1000;
A4988_Step_Motor::A4988_Step_Motor(TIM_HandleTypeDef* htim, uint32_t channel, uint32_t *_pwm_cntr)
    : htim{htim}, channel{channel}, pwm_counter{_pwm_cntr}, stp_count{}, angle_of_motor{}
{
}

A4988_Step_Motor::~A4988_Step_Motor()
{

}

void A4988_Step_Motor::init() {
    stop();

    HAL_GPIO_WritePin(a4988_dir_GPIO_Port, a4988_dir_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(a4988_rst_GPIO_Port, a4988_rst_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(a4988_ms1_GPIO_Port, a4988_ms1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(a4988_ms2_GPIO_Port, a4988_ms2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(a4988_ms3_GPIO_Port, a4988_ms3_Pin, GPIO_PIN_RESET);

}

void A4988_Step_Motor::set_speed(uint32_t steps_per_second) {
    // Timer frequency is set (PWM frequency).
    if((steps_per_second == 0))
    {
    	HAL_TIM_PWM_Stop(htim, channel);
    	stop();
    }
    else
    {
    	if(HAL_TIM_CHANNEL_STATE_READY == TIM_CHANNEL_STATE_GET(htim,0))
    	{
    		HAL_GPIO_WritePin(a4988_slp_GPIO_Port, a4988_slp_Pin, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(a4988_rst_GPIO_Port, a4988_rst_Pin, GPIO_PIN_SET);
    		HAL_TIM_PWM_Start(htim, channel);
    	}

    	uint32_t timer_frequency = HAL_RCC_GetPCLK1Freq() * 2;
    	uint32_t pwm_frequency = steps_per_second * 2; // Because two PWM periods are needed for half-step drive.
    	uint16_t pwm_period = timer_frequency / pwm_frequency - 1;
    	__HAL_TIM_SET_AUTORELOAD(htim, pwm_period);
    	__HAL_TIM_SET_COMPARE(htim, channel, pwm_period*0.5);
    }
}


A4988_Step_Motor::motor_state A4988_Step_Motor::turn_step_based(uint32_t steps_per_second, uint32_t step_cnt, bool &start) {
	// Timer frequency is set (PWM frequency).
	if(start == true && mot_state == motor_state::READY)
	{
		stp_cnt = *pwm_counter + step_cnt;
		mot_state = motor_state::BUSY;
		set_speed(steps_per_second);
	}
	else if(start == true && mot_state == motor_state::BUSY)
	{
		if(*pwm_counter >= stp_cnt)
		{
			mot_state = motor_state::READY;
			start = false;
			set_speed(0);
		}
	}

return mot_state;
}


float A4988_Step_Motor::degree_state(uint32_t steps_per_second, bool &start) {

	if(start == true )
	{
		set_speed(steps_per_second);

		if( mot_state == motor_state::READY)
		{
			stp_count_start = *pwm_counter;
			stp_count = 0;
			mot_state = motor_state::BUSY;
		}
		else
		{
			if(stp_count >= rotation)
			{
				stp_count_start = *pwm_counter;
				stp_count = 0;
			}
			stp_count = *pwm_counter - stp_count_start;
			angle_of_motor = stp_count * angle_per_step;
		}
	}
	else
	{
			mot_state = motor_state::READY;
			set_speed(0);
	}

	return angle_of_motor;
}

void A4988_Step_Motor::stop() {
	HAL_GPIO_WritePin(a4988_slp_GPIO_Port, a4988_slp_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(a4988_rst_GPIO_Port, a4988_rst_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(htim, channel, 0);
}

//void A4988_driver:: () {
//    __HAL_TIM_SET_COMPARE(htim, channel, 0);
//}



