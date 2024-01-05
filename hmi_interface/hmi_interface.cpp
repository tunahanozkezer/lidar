#include "hmi_interface.hpp"

uart_protocol::packet uart_protocol::pack_packet(uint8_t type, const std::vector<uint8_t>& data) {

	packet packet;
    packet.header[0] = 0x53;
    packet.header[1] = 0x72;
    packet.packet_type = static_cast<uint8_t>(type);
    packet.packet_size = static_cast<uint8_t>(data.size());
    packet.payload = data;
    packet.checksum = calculate_checksum(packet);

    return packet;
}

std::unique_ptr<uint8_t[]> uart_protocol::packet_to_ptr( packet packet, uint16_t& packet_size)
{
	std::unique_ptr<uint8_t[]> packed_packet(new uint8_t[packet.packet_size + 6]);

	packed_packet[0] = packet.header[0];
	packed_packet[1] = packet.header[1];
	packed_packet[2] = packet.packet_type;
	packed_packet[3] = packet.packet_size;
	std::copy(packet.payload.begin(), packet.payload.end(), packed_packet.get() + 4);
	packed_packet[packet.packet_size + 4] =  packet.checksum & 0xFF;
	packed_packet[packet.packet_size + 5] = (packet.checksum >> 8) & 0xFF;
	packet_size = packet.packet_size + 6;

	return packed_packet;
}

std::unique_ptr<uint8_t[]> uart_protocol::packet_to_ptr( std::vector<uart_protocol::packet> &packet_vector, uint16_t& packet_size)
{

	packet packet = packet_vector.front();
	packet_vector.erase(packet_vector.begin());

	return packet_to_ptr(packet, packet_size);
}

bool uart_protocol::unpack_packet(const std::vector<uint8_t>& received_data, packet& unpacked_packet) {
	 static unpack_state current_state{unpack_state::Header1};

	    for (size_t i = 0; i < received_data.size(); ++i) {
	        switch (current_state) {
	        case unpack_state::Header1:
	            if (received_data[i] == 0x53) {
	                unpacked_packet.header[0] = received_data[i];
	                current_state = unpack_state::Header2;
	            }
	            break;

	        case unpack_state::Header2:
	            if (received_data[i] == 0x72) {
	                unpacked_packet.header[1] = received_data[i];
	                current_state = unpack_state::Type;
	            } else {
	                current_state = unpack_state::Header1;
	            }
	            break;

	        case unpack_state::Type:
	            unpacked_packet.packet_type = received_data[i];
	            current_state = unpack_state::Size;
	            break;

	        case unpack_state::Size:
	            unpacked_packet.packet_size = received_data[i];
	            current_state = unpack_state::Payload;
	            break;

	        case unpack_state::Payload:
	            if (i + unpacked_packet.packet_size < received_data.size()) {
	                unpacked_packet.payload.assign(received_data.begin() + i, received_data.begin() + i + unpacked_packet.packet_size);
	                i += unpacked_packet.packet_size - 1;
	                current_state = unpack_state::Checksum1;
	            } else {
	                // Yeterli veri yok, bekle
	                return false;
	            }
	            break;

	        case unpack_state::Checksum1:
	            unpacked_packet.checksum = received_data[i];
	            current_state = unpack_state::Checksum2;
	            break;

	        case unpack_state::Checksum2:
	            unpacked_packet.checksum |= static_cast<uint16_t>(received_data[i]) << 8;
	            current_state = unpack_state::Header1;

	            return (unpacked_packet.checksum == calculate_checksum(unpacked_packet));
	        }
	    }

	    return false;
}

uint16_t uart_protocol::calculate_checksum(const packet& packet) {
    uint16_t checksum = 0;

    for (int i = 0; i < 2; ++i) {
        checksum += packet.header[i];
    }

    checksum += packet.packet_type;
    checksum += packet.packet_size;

    for (uint8_t byte : packet.payload) {
        checksum += byte;
    }

    // Little endian düzeltme
    checksum = ((checksum & 0xFF)<<8) | ((checksum & 0xFF00) >> 8);

    return checksum;
}
