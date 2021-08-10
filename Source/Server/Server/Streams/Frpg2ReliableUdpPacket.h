// Dark Souls 3 - Open Server

#pragma once

#include "Core/Utils/Endian.h"

#include <vector>
#include <string>

// Every packet can perform one of the below operations. This is basically
// just a crude reimplementation of most of the TCP operations.
enum class Frpg2ReliableUdpOpCode : uint8_t
{
    Unset           = 0x00,

    // Used to establish connection and sync sequence numbers.
    SYN             = 0x02,    

    // ??? - Seemingly unused.    
    RACK            = 0x03,                

    // Data fragment packet.
    DAT             = 0x04,

    // Hearbeat packet.
    HBT             = 0x05,

    // Connection termination.
    FIN             = 0x06,

    // Reset the connection.
    RST             = 0x07,

    // ??? - Seemingly unused.
    PT_DAT_FRAG     = 0x08,

    // Acknowledges the highest packet in the sequence that has been recieved.
    ACK             = 0x31,

    // Acknowledgement of SYN packet along with remote machines
    // owning sequence number information.
    SYN_ACK         = 0x32,

    // Acknowledges data packet and also contains a payload. This basically
    // seems to be a "reply" opcode.
    DAT_ACK         = 0x34,

    // Acknowledges connection termination.
    FIN_ACK         = 0x36,

    // ??? - Seemingly unused.
    PT_DAT_FRAG_ACK = 0x38 
};

std::string ToString(Frpg2ReliableUdpOpCode OpCode);

// What part of the packet transmission flow the stream is in.
// We only implement the server side of this protocol, we can't
// currently establish outgoing streams.
enum class Frpg2ReliableUdpStreamState
{
    Listening,
    SynRecieved,
    Established,
    Closing,
    Closed
};

#pragma pack(push,1)

// Header thats at the top of every reliable udp packet.
struct Frpg2ReliableUdpPacketHeader
{
public:

    uint16_t                magic_number = 0x02F5;
    uint8_t                 ack_counters[3] = { 0, 0, 0 };          // Not using a bitfield for these as msvc really doesn't seem to want to pack them right.
    Frpg2ReliableUdpOpCode  opcode = Frpg2ReliableUdpOpCode::Unset;
    uint8_t                 unknown_1 = 0xFF;                       // Possible congestion control. I think we can ignore this for right now.

    void GetAckCounters(uint32_t& local, uint32_t& remote) const
    {
        uint32_t upper_nibble = ((uint32_t)ack_counters[1] & 0xf0) >> 4;
        uint32_t lower_nibble = ((uint32_t)ack_counters[1] & 0x0f);

        local = (uint32_t)ack_counters[0] | (upper_nibble << 8);
        remote = (uint32_t)ack_counters[2] | (lower_nibble << 8);
    }

    void SetAckCounters(uint32_t local, uint32_t remote)
    {
        uint32_t upper_nibble = (local >> 8) & 0xF;
        uint32_t lower_nibble = (remote >> 8) & 0xF;

        ack_counters[0] = local & 0xFF;
        ack_counters[1] = (upper_nibble << 4) | (lower_nibble);
        ack_counters[2] = remote & 0xFF;
    }

};
static_assert(sizeof(Frpg2ReliableUdpPacketHeader) == 7, "Packet header is not expected size.");

// The contents of the packet if its a SYN_ACK op.
struct Frpg2ReliableUdpPacketOpCodePayload_SYN
{
public:

    uint8_t unknown[8];    

};
static_assert(sizeof(Frpg2ReliableUdpPacketOpCodePayload_SYN) == 8, "Opcode payload is not expected size.");

#pragma pack(pop)

struct Frpg2ReliableUdpPacket
{
public:

    Frpg2ReliableUdpPacketHeader Header;

    // Length is equal to the rest of the payload minus the header.
    std::vector<uint8_t> Payload;

private:
    friend class Frpg2ReliableUdpPacketStream;

    // Use for internal bookkeeping when sending/recieving the packet.
    float SendTime;

};