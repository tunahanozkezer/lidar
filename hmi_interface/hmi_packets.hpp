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
#include <cstring>
#include <tasks.hpp>

extern rtos_task os_ui;
template <class T>
class hmi_packets: uart_protocol {
public:

	enum class types : uint8_t
	{
		PERIODIC=1,
		CMD,
		SET_SPEED,
	};

	enum class cmd_types : uint8_t
	{
		STOP,
		SCAN_INF,
		SCAN_1_TIME
	};

	hmi_packets(T uart_wrapper): uart_wrapper_{uart_wrapper}{}

	void packet_periodic(uint16_t _distance_cm, float _angle_deg, motor_states mot_state)
	{
		uint16_t packet_size{};

		periodic payload{ static_cast<uint16_t>(_angle_deg*100.0),
			             _distance_cm                            ,
		                 mot_state                               };

		std::unique_ptr<uint8_t[]> byte_ptr = std::make_unique<uint8_t[]>(sizeof(payload));
		std::memcpy(byte_ptr.get(), &payload, sizeof(periodic));

		std::vector<uint8_t> payload_vector{byte_ptr.get(), byte_ptr.get() + sizeof(periodic)};
		std::unique_ptr<uint8_t[]> data_to_send{packet_to_ptr(pack_packet(static_cast<uint8_t>(types::PERIODIC), payload_vector), packet_size)};

		uart_wrapper_.sendData(data_to_send, packet_size);
	}

	void packet_parse(const std::vector<uint8_t> &received_data)
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
					os_ui.set_start_flag(false);
				}
				else if(cmd_types::SCAN_INF == static_cast<cmd_types>(unpacked_packet.payload[0]))
				{
					os_ui.set_start_flag(true);
				}
				break;
			case types::SET_SPEED:
				uint32_t pwm_freq;
				std::memcpy(&pwm_freq, unpacked_packet.payload.data(), sizeof(pwm_freq));
				os_ui.set_pwm_freq(pwm_freq);
				break;
			default:
				break;
		}
	}
private:
	T uart_wrapper_;

	struct periodic
	{
		uint16_t angle;
	    uint16_t distance;
	    motor_states mot_state;
	};
};



#endif /* HMI_PACKETS_HPP_ */
