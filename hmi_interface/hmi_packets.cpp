/*
 * hmi_packets.cpp
 *
 *  Created on: Dec 14, 2023
 *      Author: tunah
 */




#include "hmi_packets.hpp"
#include "usart.h"
#include <cstring>

extern bool start_flg;
extern uint32_t pwm_freq;
hmi_packets::hmi_packets(uart_wrapper &uart_wrapper) : uart_wrapper_{uart_wrapper}
{

}


void hmi_packets::packet_periodic(uint16_t _distance_cm, float _angle_deg, motor_state mot_state)
{

	uint16_t packet_size{};
	periodic payload{};
	payload.distance = _distance_cm;
	payload.angle    = static_cast<uint16_t>(_angle_deg*100.0);
	payload.mot_state = mot_state;

	uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(&payload);

	std::vector<uint8_t> dataToSend{byte_ptr, byte_ptr + sizeof(periodic)};

	std::unique_ptr<uint8_t[]> dene{packet_to_ptr(pack_packet(static_cast<uint8_t>(types::PERIODIC), dataToSend), packet_size)};
	uart_wrapper_.sendData(dene, packet_size);

}

void hmi_packets::packet_parse(const std::vector<uint8_t> &received_data)
{
	packet unpacked_packet{};

	if(!unpack_packet(received_data, unpacked_packet))
	{
		return;
	}

	switch (static_cast<types>(unpacked_packet.packet_type)) {
		case types::CMD:
			if(cmd_types::STOP == static_cast<cmd_types>(unpacked_packet.payload[0]))
			{
				start_flg = false;
			}
			else if(cmd_types::SCAN_INF == static_cast<cmd_types>(unpacked_packet.payload[0]))
			{
				start_flg = true;
			}
			break;
		case types::SET_SPEED:
			std::memcpy(&pwm_freq, unpacked_packet.payload.data(), sizeof(pwm_freq));
			break;
		default:
			break;
	}
}
