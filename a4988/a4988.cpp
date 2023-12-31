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

A4988_Step_Motor::A4988_Step_Motor(std::shared_ptr<TIM_HandleTypeDef> timer, uint32_t channel, std::shared_ptr<uint32_t> pwm_counter, step_resolution step_reso)
    :  timer_{timer},  pwm_counter_{pwm_counter}, channel_{channel}, step_count_{}, angle_of_motor{}, rotation{1000}
{
    stop();
    set_step_resolution(step_reso);
    HAL_GPIO_WritePin(a4988_dir_GPIO_Port, a4988_dir_Pin, GPIO_PIN_RESET);
}

void A4988_Step_Motor::set_speed(uint32_t steps_per_second) {
    // Timer frequency is set (PWM frequency).
    if((steps_per_second == 0))
    {
    	HAL_TIM_PWM_Stop(timer_.get(), channel_);
    	stop();
    }
    else
    {
    	if(HAL_TIM_CHANNEL_STATE_READY == TIM_CHANNEL_STATE_GET(timer_.get(),0))
    	{
    		HAL_GPIO_WritePin(a4988_slp_GPIO_Port, a4988_slp_Pin, GPIO_PIN_SET);
    		HAL_GPIO_WritePin(a4988_rst_GPIO_Port, a4988_rst_Pin, GPIO_PIN_SET);
    		HAL_TIM_PWM_Start(timer_.get(), channel_);
    	}

    	uint32_t timer_frequency = HAL_RCC_GetPCLK1Freq() * 2;
    	uint32_t pwm_frequency = steps_per_second * 2; // Because two PWM periods are needed for half-step drive.
    	uint16_t pwm_period = timer_frequency / pwm_frequency - 1;
    	__HAL_TIM_SET_AUTORELOAD(timer_.get(), pwm_period);
    	__HAL_TIM_SET_COMPARE(timer_.get(), channel_, pwm_period*0.5);
    }
}


motor_state A4988_Step_Motor::turn_step_based(uint32_t steps_per_second, uint32_t step_count, bool& start) {
	// Timer frequency is set (PWM frequency).
	if(start == true && motor_state_ == motor_state::Ready)
	{
		step_count_ = *pwm_counter_ + step_count_;
		motor_state_ = motor_state::Busy;
		set_speed(steps_per_second);
	}
	else if(start == true && motor_state_ == motor_state::Busy)
	{
		if(*pwm_counter_ >= step_count_)
		{
			motor_state_ = motor_state::Ready;
			start = false;
			set_speed(0);
		}
	}

return motor_state_;
}


float A4988_Step_Motor::degree_state(uint32_t steps_per_second, bool &start, motor_state &motor_state) {

	if(start == true )
	{
		set_speed(steps_per_second);

		if( motor_state_ == motor_state::Ready)
		{
			step_count_ = *pwm_counter_;
			step_count_ = 0;
			motor_state_ = motor_state::Busy;
		}
		else
		{
			if(step_count_ >= rotation)
			{
				stp_count_start_ = *pwm_counter_;
				step_count_ = 0;
			}
			step_count_ = *pwm_counter_ - stp_count_start_;
			angle_of_motor = step_count_ * angle_per_step;
		}
	}
	else
	{
		motor_state_ = motor_state::Ready;
		set_speed(0);
	}

	motor_state = motor_state_;
	return angle_of_motor;
}

void A4988_Step_Motor::stop() {
	HAL_GPIO_WritePin(a4988_slp_GPIO_Port, a4988_slp_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(a4988_rst_GPIO_Port, a4988_rst_Pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(timer_.get(), channel_, 0);
}


void A4988_Step_Motor::set_step_resolution(step_resolution step_reso)
{
	switch (step_reso) {
		case step_resolution::FULL_STEP:
			HAL_GPIO_WritePin(a4988_ms1_GPIO_Port, a4988_ms1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(a4988_ms2_GPIO_Port, a4988_ms2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(a4988_ms3_GPIO_Port, a4988_ms3_Pin, GPIO_PIN_RESET);

			break;
		case step_resolution::HALF_STEP:
			HAL_GPIO_WritePin(a4988_ms1_GPIO_Port, a4988_ms1_Pin, GPIO_PIN_SET  );
			HAL_GPIO_WritePin(a4988_ms2_GPIO_Port, a4988_ms2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(a4988_ms3_GPIO_Port, a4988_ms3_Pin, GPIO_PIN_RESET);
			break;
		case step_resolution::QUARTER_STEP:
			HAL_GPIO_WritePin(a4988_ms1_GPIO_Port, a4988_ms1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(a4988_ms2_GPIO_Port, a4988_ms2_Pin, GPIO_PIN_SET  );
			HAL_GPIO_WritePin(a4988_ms3_GPIO_Port, a4988_ms3_Pin, GPIO_PIN_RESET);
			break;
		case step_resolution::EIGHT_STEP:
			HAL_GPIO_WritePin(a4988_ms1_GPIO_Port, a4988_ms1_Pin, GPIO_PIN_SET  );
			HAL_GPIO_WritePin(a4988_ms2_GPIO_Port, a4988_ms2_Pin, GPIO_PIN_SET  );
			HAL_GPIO_WritePin(a4988_ms3_GPIO_Port, a4988_ms3_Pin, GPIO_PIN_RESET);
			break;
		case step_resolution::SIXTEENTH_STEP:
			HAL_GPIO_WritePin(a4988_ms1_GPIO_Port, a4988_ms1_Pin, GPIO_PIN_SET  );
			HAL_GPIO_WritePin(a4988_ms2_GPIO_Port, a4988_ms2_Pin, GPIO_PIN_SET  );
			HAL_GPIO_WritePin(a4988_ms3_GPIO_Port, a4988_ms3_Pin, GPIO_PIN_SET  );
			break;
		default:
			break;
	}
	rotation = 1000 * static_cast<uint32_t>(step_reso);
	angle_per_step = 360.0/static_cast<float>(rotation);
}

