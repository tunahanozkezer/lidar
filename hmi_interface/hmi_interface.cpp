#include <numeric>
#include <iterator>
#include "hmi_interface.hpp"

uart_protocol::packet uart_protocol::pack_packet(uint8_t type, const std::vector<uint8_t>& data) {

	auto packet = uart_protocol::packet{
		head_1, // header1
		head_2, // header2
		type, // packet_type
        static_cast<uint8_t>(data.size()), // packet_size
        data, // payload
        0 // checksum
    };
    packet.checksum = calculate_checksum(packet);

    return packet;
}

std::unique_ptr<uint8_t[]> uart_protocol::packet_to_ptr( const packet &packet, uint16_t& packet_size)
{

	std::unique_ptr<uint8_t[]> packed_packet = std::make_unique<uint8_t[]>(packet.packet_size + protocol_all_size);
	packed_packet[0] = packet.header_1;
	packed_packet[1] = packet.header_2;
	packed_packet[2] = packet.packet_type;
	packed_packet[3] = packet.packet_size;
	std::copy(packet.payload.begin(), packet.payload.end(), packed_packet.get() + protocol_head_size);
	packed_packet[packet.packet_size + protocol_head_size] =  packet.checksum & 0xFF;
	packed_packet[packet.packet_size + protocol_head_size + 1] = (packet.checksum >> 8) & 0xFF;
	packet_size = packet.packet_size + protocol_all_size;

	return packed_packet;
}

std::unique_ptr<uint8_t[]> uart_protocol::packet_to_ptr(std::vector<uart_protocol::packet>& packet_vector, uint16_t& packet_size) {
    if (packet_vector.empty()) {
        return nullptr;
    }

    auto packet = std::move(packet_vector.front());
    packet_vector.erase(packet_vector.begin());

    return packet_to_ptr(packet, packet_size);
}


bool uart_protocol::unpack_packet(const std::vector<uint8_t>& received_data, packet& unpacked_packet) {

    if(received_data.size() == 0)
    {
    	return false;
    }

    static unpack_state current_state{unpack_state::Header1};
    for (auto it = received_data.begin(); it != received_data.end(); ++it) {
        auto& data = *it;
		switch (current_state) {
        case unpack_state::Header1:
            if (data == head_1) {
                unpacked_packet.header_1 = data;
                current_state = unpack_state::Header2;
            }
            break;

        case unpack_state::Header2:
            if (data == head_2) {
                unpacked_packet.header_2 = data;
                current_state = unpack_state::Type;
            } else {
                current_state = unpack_state::Header1;
            }
            break;

        case unpack_state::Type:
            unpacked_packet.packet_type = data;
            current_state = unpack_state::Size;
            break;

        case unpack_state::Size:
            unpacked_packet.packet_size = data;
            current_state = unpack_state::Payload;
            break;

        case unpack_state::Payload:
           if (std::distance(it, received_data.end()) >= unpacked_packet.packet_size) {
                unpacked_packet.payload.assign(it, it + unpacked_packet.packet_size);
                std::advance(it, unpacked_packet.packet_size - 1);
                current_state = unpack_state::Checksum1;
            } else {
                return false;
            }
            break;

        case unpack_state::Checksum1:
            unpacked_packet.checksum = data;
            current_state = unpack_state::Checksum2;
            break;

        case unpack_state::Checksum2:
            unpacked_packet.checksum |= static_cast<uint16_t>(data) << 8;
            current_state = unpack_state::Header1;

            return (unpacked_packet.checksum == calculate_checksum(unpacked_packet));
        }
    }
    return false;
}

uint16_t uart_protocol::calculate_checksum(const packet& packet) {
    uint16_t checksum {packet.header_1};
    checksum += packet.header_2;
    checksum += packet.packet_type;
    checksum += packet.packet_size;
    checksum += std::accumulate(packet.payload.begin(), packet.payload.end(), 0);
    return checksum;
}
