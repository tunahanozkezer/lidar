/*
 * tf_luna.hpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */

#ifndef TF_LUNA_HPP_
#define TF_LUNA_HPP_

#include <memory>

enum class packet_id :uint8_t
{
	ID_GET_VERSION       =0x01,
	ID_SOFT_RESET        =0x02,
	ID_SAMPLE_FREQ       =0x03,
	ID_SAMPLE_TRIG       =0x04,
	ID_OUTPUT_FORMAT     =0x05,
	ID_BAUD_RATE         =0x06,
	ID_OUTPUT_EN         =0x07,
	ID_FRAME_CHECKSUM_EN =0x08,
	ID_I2C_SLAVE_ADDR    =0x0B,
	ID_RESTORE_DEFAULT   =0x10,
	ID_SAVE_SETTINGS     =0x11,
	ID_READ_MANU_BIN     =0x12,
	ID_GET_FULL_VERSION  =0x14,
	ID_AMP_THRESHOLD     =0x22,
	ID_TIMESTAMP_SYNC    =0x31,
	ID_DIST_LIMIT        =0x3A,
	ID_ON_OFF_MODE       =0x3B,
	ID_LOW_SAMPLE_RATE   =0x3E,
	ID_GET_CONFIG_PARA   =0x3F
};

enum class output_en_state :bool
{
	DISABLE       =0x00,
	ENABLE        =0x01
};

enum class chekcsum_status :bool
{
	DONT_CALCULATE,
	CALCULATE
};

template <class uart_type>
class tf_luna
{
public:

	uint16_t distance_cm_u16;
	uint16_t amp_u16;
	float temp_c_f32;

	tf_luna(uart_type uart_wrapper): uart_wrapper_{uart_wrapper}{}

	void  output_control(output_en_state state)
	{
		const std::vector<uint8_t> payload{static_cast<uint8_t>(state)};
		packet send_packet{pack_packet(payload)};

		std::unique_ptr<uint8_t[]> p_packet_to_send{std::make_unique<uint8_t[]>(send_packet.len_u8)};

		p_packet_to_send[0] = send_packet.head_u8;
		p_packet_to_send[1] = send_packet.len_u8;
		p_packet_to_send[2] = static_cast<uint8_t>(send_packet.id_u8);
		p_packet_to_send[3] = send_packet.payload[0];
		p_packet_to_send[4] = send_packet.cheksum;

		uart_wrapper_.sendData(p_packet_to_send, send_packet.len_u8);
	}

	uint16_t output_control()
	{
		if(!data_recieved){
			output_control(output_en_state::ENABLE);
			return 200;
		}
		else
		{
			return 10;
		}
	}

	void parse_byte(std::vector<uint8_t> received_data) {

		if(received_data.size() == 0)
	    {
	    	return;
	    }

	    uint16_t tmp_distance_cm_u16     {};
	    uint16_t tmp_amp_u16             {};
	    uint16_t tmp_temperature_raw_u16 {};
	    uint8_t chekcsum_u8              {};

	    for (auto it = received_data.begin(); it != received_data.end(); ++it) {
	    	auto& byte_u8 = *it;

	        switch (parse_state) {
	            case parse_state_t::HEAD_1:
	                if (byte_u8 == 0x59) {
	                    chekcsum_u8 += byte_u8;
	                    parse_state = parse_state_t::HEAD_2;
	                }
	                break;

	            case parse_state_t::HEAD_2:
	                if (byte_u8 == 0x59) {
	                    chekcsum_u8 += byte_u8;
	                    parse_state = parse_state_t::DIST_L;
	                } else {
	                    chekcsum_u8 = 0;
	                    parse_state = parse_state_t::HEAD_1;
	                }
	                break;

	            case parse_state_t::DIST_L:
	                chekcsum_u8 += byte_u8;
	                tmp_distance_cm_u16 = byte_u8;
	                parse_state = parse_state_t::DIST_H;
	                break;

	            case parse_state_t::DIST_H:
	                chekcsum_u8 += byte_u8;
	                tmp_distance_cm_u16 |= ((uint16_t)byte_u8) << 8;
	                parse_state = parse_state_t::AMP_L;
	                break;

	            case parse_state_t::AMP_L:
	                chekcsum_u8 += byte_u8;
	                tmp_amp_u16 = byte_u8;
	                parse_state = parse_state_t::AMP_H;
	                break;

	            case parse_state_t::AMP_H:
	                chekcsum_u8 += byte_u8;
	                tmp_amp_u16 |= ((uint16_t)byte_u8) << 8;
	                parse_state = parse_state_t::TEMP_L;
	                break;

	            case parse_state_t::TEMP_L:
	                chekcsum_u8 += byte_u8;
	                tmp_temperature_raw_u16 = byte_u8;
	                parse_state = parse_state_t::TEMP_H;
	                break;

	            case parse_state_t::TEMP_H:
	                chekcsum_u8 += byte_u8;
	                tmp_temperature_raw_u16 |= ((uint16_t)byte_u8) << 8;
	                parse_state = parse_state_t::CHEKSUM;
	                break;

	            case parse_state_t::CHEKSUM:
	                if (chekcsum_u8 == byte_u8) {
	                    distance_cm_u16 = tmp_distance_cm_u16;
	                    amp_u16 = tmp_amp_u16;
	                    temp_c_f32 = (tmp_temperature_raw_u16 / 8) - 256;
	                    data_recieved = true;
	                } else {
	                }

	                chekcsum_u8 = 0;
	                parse_state = parse_state_t::HEAD_1;
	                break;
	        }
	    }
	    received_data.clear();
	}
private:

	constexpr static uint8_t header = 0x5A;
	uart_type uart_wrapper_;
	enum class parse_state_t
	{
		HEAD_1,
		HEAD_2,
		DIST_L,
		DIST_H,
		AMP_L ,
		AMP_H ,
		TEMP_L,
		TEMP_H,
		CHEKSUM
	};
	parse_state_t parse_state;
	bool data_recieved;


	struct packet
	{
		uint8_t              head_u8 ;
		uint8_t              len_u8  ;
		packet_id            id_u8   ;
		std::vector<uint8_t> payload ;
		uint8_t              cheksum ;
	};

	packet pack_packet(const std::vector<uint8_t>& data, packet_id tmp_packet_id = packet_id::ID_OUTPUT_EN, chekcsum_status cheksum_stat = chekcsum_status::DONT_CALCULATE)
	{
		auto send_packet = packet{
			header       , // header
			5            , // len
			tmp_packet_id, //status
			data         , // payload
			0              // checksum
		};

		if(cheksum_stat == chekcsum_status::CALCULATE)
		{
			send_packet.cheksum = calculate_checksum(send_packet);
		}

		return send_packet;
	}

	uint8_t calculate_checksum(const packet &packet_t)
	{
		uint8_t calculated_checksum {};
		calculated_checksum += packet_t.head_u8;
		calculated_checksum += packet_t.len_u8;
		calculated_checksum += static_cast<uint8_t>(packet_t.id_u8);

		for(unsigned int i = 0; i<packet_t.payload.size(); i++)
		{
			calculated_checksum += packet_t.payload[i];
		}
		return calculated_checksum;
	}
};


#endif /* TF_LUNA_HPP_ */
