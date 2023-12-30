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
extern uint32_t pres;
hmi_packets::hmi_packets()
{

}


uart_protocol::packet hmi_packets::packet_periodic(uint16_t _distance_cm, float _angle_deg)
{

	periodic payload{};
	payload.distance = _distance_cm;
	payload.angle    = static_cast<uint16_t>(_angle_deg*100.0);

	uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(&payload);

	std::vector<uint8_t> dataToSend{byte_ptr, byte_ptr + sizeof(periodic)};

	return uart_protocol::pack_packet(static_cast<uint8_t>(types::PERIODIC), dataToSend);

}

void hmi_packets::packet_parse(uart_protocol::packet &packet)
{
	switch (static_cast<types>(packet.packet_type)) {
		case types::CMD:
			if(cmd_types::STOP == static_cast<cmd_types>(packet.payload[0]))
			{
				start_flg = false;
			}
			else if(cmd_types::SCAN_INF == static_cast<cmd_types>(packet.payload[0]))
			{
				start_flg = true;
			}
			break;
		case types::SET_SPEED:
			std::memcpy(&pres, packet.payload.data(), sizeof(pres));
			break;
		default:
			break;
	}
}
