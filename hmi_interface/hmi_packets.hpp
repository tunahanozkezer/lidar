/*
 * hmi_packets.hpp
 *
 *  Created on: Dec 17, 2023
 *      Author: tunah
 */

#ifndef HMI_PACKETS_HPP_
#define HMI_PACKETS_HPP_

#include <hmi_interface.hpp>
#include <a4988.hpp>
#include <uart_wrapper.hpp>

class hmi_packets: uart_protocol {
public:

	enum class types : uint8_t
	{
		PERIODIC=1,
		CMD,
		SET_SPEED,
	};


	struct packet_for_uart
	{

	};
	hmi_packets(uart_wrapper &uart_wrapper);

	void packet_periodic(uint16_t _distance_cm, float _angle_deg, motor_state mot_state);
	static void packet_parse(const std::vector<uint8_t> &received_data);
private:
	uart_wrapper &uart_wrapper_;

	struct periodic
	{
		uint16_t angle;
	    uint16_t distance;
	    motor_state mot_state;
	};

	enum class cmd_types : uint8_t
	{
		STOP,
		SCAN_INF,
		SCAN_1_TIME
	};

    // Add any other necessary member functions or variables
};



#endif /* HMI_PACKETS_HPP_ */
