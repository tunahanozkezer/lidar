/*
 * tf_luna.cpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */


#include "tf_luna.hpp"
#include "stdlib.h"
#include "usart.h"

tf_luna::tf_luna() : parse_state{parse_state_t::HEAD_1}
{

}

void  tf_luna::output_control(output_en_state state)
{
	packet send_packet(1);

	send_packet.head_u8     = 0x5A;
	send_packet.len_u8      = 5;
	send_packet.id_u8       = packet_id::ID_OUTPUT_EN;
	send_packet.data_u8[0]  = static_cast<uint8_t>(state);
	send_packet.chek_sum_u8 = 0;//calculate_checksum(send_packet);

	 std::unique_ptr<uint8_t[]> p_packet_to_send(new uint8_t[send_packet.len_u8]);

	 p_packet_to_send[0] = send_packet.head_u8;
	 p_packet_to_send[1] = send_packet.len_u8;
	 p_packet_to_send[2] = static_cast<uint8_t>(send_packet.id_u8);
	 p_packet_to_send[3] = send_packet.data_u8[0];
	 p_packet_to_send[4] = send_packet.chek_sum_u8;

	 HAL_UART_Transmit_DMA(&huart1, p_packet_to_send.get(), send_packet.len_u8);

//	xQueueSend(tx_qu, &verileer, 10);

}


void tf_luna::parse_byte(std::vector<uint8_t> &data_vector) {
    uint16_t tmp_distance_cm_u16 = 0;
    uint16_t tmp_amp_u16 = 0;
    uint16_t tmp_temperature_raw_u16 = 0;
    uint8_t chekcsum_u8 = 0;

    for (size_t i = 0; i < data_vector.size(); ++i) {
        uint8_t byte_u8 = data_vector[i];

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
                    // Veri başarıyla işlendi
                } else {
                    // Checksum uyuşmazlığı
                }

                chekcsum_u8 = 0;
                parse_state = parse_state_t::HEAD_1;
                break;
        }
    }
    // İşlenen verileri temizle
    data_vector.clear();
}

uint8_t tf_luna::calculate_checksum(const packet &packet_t)
{
	uint8_t calculated_checksum = 0;
	calculated_checksum += packet_t.head_u8;
	calculated_checksum += packet_t.len_u8;
	calculated_checksum += static_cast<uint8_t>(packet_t.id_u8);

	for(int i = 0; i<packet_t.len_u8; i++)
	{
		calculated_checksum += packet_t.data_u8[i];
	}
	return calculated_checksum;

}
