/*
 * hmi_packets.hpp
 *
 *  Created on: Dec 17, 2023
 *      Author: tunah
 */

#ifndef HMI_PACKETS_HPP_
#define HMI_PACKETS_HPP_

#include <hmi_interface.hpp>


class hmi_packets {
public:
	hmi_packets();

	static uint8_t* packet_periodic(uint16_t _distance_cm, uint16_t _signal_quality, uint16_t _angle_deg);
private:
    static std::unique_ptr<uint8_t[]> buffer; // The byte buffer

    struct periodic {
        uint16_t distance_cm;
        uint16_t signal_quality;
        uint16_t  angle_deg;
    };

    static periodic periodic_pack;

    // Add any other necessary member functions or variables
};



#endif /* HMI_PACKETS_HPP_ */
