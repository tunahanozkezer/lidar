/*
 * hmi_interface.cpp
 *
 *  Created on: Dec 14, 2023
 *      Author: tunah
 */

#include "hmi_interface.hpp"
#include <cstring>

using namespace hmi_interface;

packet::packet()
{

}

uint8_t* packet::get_packed(uint8_t *_payload, uint8_t _size, types _type)
{
	raw_value = std::make_unique<uint8_t[]>(sizeof(_size+interface_size));

	type = _type;
	size = _size;
	const auto seven_nation_army = header;
//	raw_value(new uint8_t[header_size + size + checksum_size]);

	std::memcpy(raw_value.get()                               ,&seven_nation_army , sizeof(header));
	std::memcpy(raw_value.get()+sizeof(header)                ,&type              , sizeof(type  ));
	std::memcpy(raw_value.get()+sizeof(header) + sizeof(type) ,&size              , sizeof(size  ));
	std::memcpy(raw_value.get()+header_size                   ,_payload           , size          );

	const uint16_t chekcsum =calculate_checksum(raw_value.get(), (size + header_size));

	std::memcpy(raw_value.get()+header_size + size-1           ,&chekcsum , sizeof(chekcsum));

	return raw_value.get();
}

uint16_t packet::calculate_checksum(uint8_t *data_for_calculate, size_t size)
	{
		uint16_t chekcsum{};

		for(size_t i = 0; i< size; i++)
		{
			chekcsum += data_for_calculate[i];
		}

		return chekcsum;
	}
