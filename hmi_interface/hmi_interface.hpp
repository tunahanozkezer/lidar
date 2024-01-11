#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <vector>
#include <memory>

class uart_protocol {

public:

	struct packet {
		uint8_t header_1;
		uint8_t header_2;
		uint8_t packet_type;
		uint8_t packet_size;
		std::vector<uint8_t> payload;
		uint16_t checksum;
	};
protected:
	static bool unpack_packet(const std::vector<uint8_t>& received_data, packet& unpacked_packet);
	static uart_protocol::packet pack_packet(uint8_t type, const std::vector<uint8_t>& data);
	static std::unique_ptr<uint8_t[]> packet_to_ptr( const packet &packet, uint16_t& packet_size);
	static std::unique_ptr<uint8_t[]> packet_to_ptr( std::vector<uart_protocol::packet> &packet_vector, uint16_t& packet_size);

private:
	static constexpr uint8_t head_1{0x53} ;
	static constexpr uint8_t head_2{0x72} ;
	static constexpr auto protocol_head_size{sizeof(packet::header_1) + sizeof(packet::header_2) + sizeof(packet::packet_type) + sizeof(packet::packet_size)};
	static constexpr auto protocol_all_size {protocol_head_size  + sizeof(packet::checksum)};

	enum class unpack_state {
		Header1,
		Header2,
		Type,
		Size,
		Payload,
		Checksum1,
		Checksum2
	};
	static uint16_t calculate_checksum(const packet& packet);


};

#endif // UART_PROTOCOL_H
