/*
 * hmi_packets.cpp
 *
 *  Created on: Dec 14, 2023
 *      Author: tunah
 */




#include "hmi_packets.hpp"
#include "usart.h"
#include <cstring>  // Include for std::memcpy

hmi_packets::hmi_packets()
{

}


uint8_t* hmi_packets::packet_periodic(uint16_t _distance_cm, uint16_t _signal_quality, uint16_t _angle_deg)
{
	periodic_pack = { _distance_cm, _signal_quality, _angle_deg };

    buffer = std::make_unique<uint8_t[]>(sizeof(periodic_pack));
    std::memcpy(buffer.get(), &periodic_pack, sizeof(periodic_pack));


    return buffer.get();
}
