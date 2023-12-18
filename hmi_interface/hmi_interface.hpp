/*
 * hmi_interface.hpp
 *
 *  Created on: Dec 14, 2023
 *      Author: tunah
 */

#ifndef HMI_INTERFACE_HPP_
#define HMI_INTERFACE_HPP_

#include <stdint.h>
#include <memory>

namespace hmi_interface
{
using namespace std;

enum class types : uint8_t
{
	VERSION,
	CHECK,
	PERIODIC
};
struct packet
{
	packet();
	uint8_t* get_packed(uint8_t *_payload, uint8_t _size, types _type);

	std::unique_ptr<uint8_t[]> raw_value;

	static constexpr uint16_t header = 0x5372;
	types    type                            ;
	uint8_t  size                            ;
	std::unique_ptr<uint8_t[]> payload       ;
	uint16_t checksum                        ;

	static constexpr auto header_size   = sizeof(header) + sizeof(type)+ sizeof(size);
	static constexpr auto checksum_size = sizeof(checksum);
	static constexpr auto interface_size = header_size + checksum_size;
private:



	uint16_t calculate_checksum(uint8_t *data_for_calculate, size_t size);
};

}

#endif /* HMI_INTERFACE_HPP_ */
