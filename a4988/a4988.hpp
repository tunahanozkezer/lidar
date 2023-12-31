/*
 * a4988.hpp
 *
 *  Created on: Dec 25, 2023
 *      Author: tunah
 */

#ifndef A4988_HPP_
#define A4988_HPP_

#include "tim.h"
#include <memory>

enum class motor_state : uint16_t {
    Ready,
    Busy
};

enum class step_resolution : uint8_t{
	FULL_STEP      = 1 ,
	HALF_STEP      = 2 ,
	QUARTER_STEP   = 4 ,
	EIGHT_STEP     = 8 ,
	SIXTEENTH_STEP = 16
};

class A4988_Step_Motor  {
public:

    motor_state motor_state_;

	A4988_Step_Motor(std::shared_ptr<TIM_HandleTypeDef> timer, uint32_t channel, std::shared_ptr<uint32_t> pwm_counter, step_resolution step_reso = step_resolution::SIXTEENTH_STEP);
    virtual ~A4988_Step_Motor() = default;

    A4988_Step_Motor(const A4988_Step_Motor&) = delete;
    A4988_Step_Motor& operator=(const A4988_Step_Motor&) = delete;

    void set_speed(uint32_t steps_per_second);
    motor_state turn_step_based(uint32_t steps_per_second, uint32_t step_count, bool& start);
    float degree_state(uint32_t steps_per_second, bool &start, motor_state &motor_state);
    void stop();
private:
    float angle_per_step;
    std::shared_ptr<TIM_HandleTypeDef> timer_;
    std::shared_ptr<uint32_t> pwm_counter_;
    uint32_t channel_;
    uint32_t stp_count_start_;
    uint32_t step_count_;
    float angle_of_motor;
    uint32_t rotation;

    void set_step_resolution(step_resolution step_reso = step_resolution::SIXTEENTH_STEP);
};



#endif /* A4988_HPP_ */
