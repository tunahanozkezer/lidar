/*
 * tf_luna.hpp
 *
 *  Created on: Nov 7, 2023
 *      Author: tunah
 */

#ifndef TF_LUNA_HPP_
#define TF_LUNA_HPP_

#include "stdint.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "queue"

using namespace std;

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

struct uart_veri
{
	uint8_t p_veri[255];
	uint16_t veri_boyutu;
};

class tf_luna
{
public:

	uint16_t distance_cm_u16;
	uint16_t amp_u16;
	float temp_c_f32;


	tf_luna(uint8_t deneme);
	void  version_downward();
	void version_upward();
	QueueHandle_t tx_qu;
	void parse_byte(queue<uint8_t> &data_qu);
private:

enum class parse_state_t
{
	HEAD_1,
	HEAD_2,
	DIST_L,
	DIST_H,
	AMP_L,
	AMP_H,
	TEMP_L,
	TEMP_H,
	CHEKSUM
};
parse_state_t parse_state;

struct packet
{
    uint8_t   head_u8     ;
	uint8_t   len_u8      ;
	packet_id id_u8       ;
	uint8_t   veri_u8[255];
	uint8_t   chek_sum_u8 ;
};

uint8_t calculate_checksum(const packet &packet_t);
};


#endif /* TF_LUNA_HPP_ */
