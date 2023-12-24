#include "hmi_interface.hpp"

std::unique_ptr<uint8_t[]> uart_protocol::pack_packet(packet_type type, const std::vector<uint8_t>& data, uint32_t& packet_size) {
    packet packet;
    packet_size = 0;
    packet.header[0] = 0x53;
    packet.header[1] = 0x72;
    packet.packet_type = static_cast<uint8_t>(type);
    packet.packet_size = static_cast<uint8_t>(data.size());
    packet.payload = data;
    packet.checksum = calculate_checksum(packet);

    packet_size = 6 + packet.packet_size; // Header + Tip + Boyut + Checksum1 + Checksum2

    std::unique_ptr<uint8_t[]> packed_packet(new uint8_t[packet_size]);
    packed_packet[0] = packet.header[0];
    packed_packet[1] = packet.header[1];
    packed_packet[2] = packet.packet_type;
    packed_packet[3] = packet.packet_size;
    std::copy(packet.payload.begin(), packet.payload.end(), packed_packet.get() + 4);
    packed_packet[packet_size - 2] = packet.checksum & 0xFF;
    packed_packet[packet_size - 1] = (packet.checksum >> 8) & 0xFF;

    return packed_packet;
}

bool uart_protocol::unpack_packet(const std::vector<uint8_t>& received_data, packet& unpacked_packet) {
    if (received_data.size() < 6) {
        // Minimum 6 byte gerekli (header, tip, boyut, checksum1, checksum2)
        return false;
    }

    unpacked_packet.header[0] = received_data[0];
    unpacked_packet.header[1] = received_data[1];
    unpacked_packet.packet_type = received_data[2];
    unpacked_packet.packet_size = received_data[3];

    if (received_data.size() != (unpacked_packet.packet_size + 6)) {
        // Boyut uyuşmazlığı
        return false;
    }

    unpacked_packet.payload.assign(received_data.begin() + 4, received_data.begin() + 4 + unpacked_packet.packet_size);
    unpacked_packet.checksum = (received_data[4 + unpacked_packet.packet_size] << 8) | received_data[5 + unpacked_packet.packet_size];

    // Checksum kontrolü
    return (calculate_checksum(unpacked_packet) == unpacked_packet.checksum);
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
