#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <iostream>
#include <vector>
#include <memory>

class uart_protocol {
public:
    // Paket tipleri enum'u
    enum packet_type {
        data_packet = 0x01,
        control_packet = 0x02
        // İhtiyaca göre diğer paket tiplerini ekleyebilirsiniz
    };

    // Paket yapısı
    struct packet {
        uint8_t header[2];
        uint8_t packet_type;
        uint8_t packet_size;
        std::vector<uint8_t> payload;
        uint16_t checksum;
    };

    // Paketleme işlemi
    static std::unique_ptr<uint8_t[]> pack_packet(packet_type type, const std::vector<uint8_t>& data, uint32_t& packet_size);

    // Paketi çözme işlemi
    static bool unpack_packet(const std::vector<uint8_t>& received_data, packet& unpacked_packet);

private:
    enum class unpack_state {
        Header1,
        Header2,
        Type,
        Size,
        Payload,
        Checksum1,
        Checksum2
    };

    // Checksum hesaplama
    static uint16_t calculate_checksum(const packet& packet);
};

#endif // UART_PROTOCOL_H
