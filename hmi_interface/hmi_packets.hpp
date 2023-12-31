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

class hmi_packets {
public:

	enum class types : uint8_t
	{
		PERIODIC=1,
		CMD,
		SET_SPEED,
	};



	hmi_packets();

	static uart_protocol::packet packet_periodic(uint16_t _distance_cm, float _angle_deg, motor_state mot_state);
	static void packet_parse(uart_protocol::packet &packet);
private:
//    static std::unique_ptr<uint8_t[]> buffer; // The byte buffer


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
