/*
 * a4988.hpp
 *
 *  Created on: Dec 25, 2023
 *      Author: tunah
 */

#ifndef A4988_HPP_
#define A4988_HPP_

#include "tim.h"

class A4988_Step_Motor {
public:

	enum class motor_state
	{
		READY,
		BUSY
	};

    A4988_Step_Motor(TIM_HandleTypeDef* htim, uint32_t channel, uint32_t *_pwm_cntr);
    ~A4988_Step_Motor();

    void init();
    void set_speed(uint32_t steps_per_second);
    motor_state turn_step_based(uint32_t steps_per_second, uint32_t step_cnt, bool &start);
    float degree_state(uint32_t steps_per_second, bool &start);
    void stop();

private:
    static constexpr float angle_per_step = 360.0/1000.0;

    TIM_HandleTypeDef* htim;
    uint32_t channel;
    uint32_t stp_cnt;
    uint32_t *pwm_counter;
    motor_state mot_state;

    uint32_t stp_count;
    uint32_t stp_count_start;
    double angle_of_motor;
};



#endif /* A4988_HPP_ */
