#include "Helpers.h"
#include "Config.h"
#include "DataStructure.h"
#include "Global.h"

#include <cassert>
#include <cstring>

std::shared_ptr<Helpers::Logger> logger;  // global logger

namespace Helpers {
    int window_size_ = 0, max_seqnum_ = 0;

    void Logger::print_packet(std::string description, const Packet& packet) {
        pUtils->printPacket(description.c_str(), packet);
    }

    void Logger::common_info(std::string info) {
        std::cout << "[common info] " << info << std::endl;
    }

    void Logger::print_window(int start, int end, std::string hint) {
        std::string window = "[ ";
        assert(max_seqnum_ != 0);  // make sure % is valid
        for (auto i = start; i != end; i = (i + 1) % max_seqnum_) {
            window += std::to_string(i) + " ";
        }
        window += "]";
        std::cout << "[" + hint + "] " << window << std::endl;
    }

    void Logger::print_state(const std::vector<bool>& state, int start, int end) {
        std::string window = "[ ";
        assert(max_seqnum_ != 0);  // make sure % is valid
        for (auto i = start; i != end; i = (i + 1) % max_seqnum_) {
            window += std::to_string(state[i]) + " ";
        }
        window += "]";
        std::cout << "[recv-state]  " << window << std::endl;
    }

    bool check_in_range(int target, int lb, int ub) {
        if (lb <= ub) {
            return target >= lb && target < ub;
        } else {
            if (target >= lb) return target < ub + max_seqnum_;
            else return target < ub;
        }
        return false;
    }

    bool check_in_window(int target, int base) {
        if (target < base) {
            return target < (base + window_size_) % max_seqnum_;
        } else {
            return target < base + window_size_;
        }
    }

    void init(int window_size, int max_seqnum) {
        window_size_ = window_size;
        max_seqnum_ = max_seqnum;
    }

    std::shared_ptr<Packet> make_msg_packet(const Message& message, int seqnum) {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        packet->acknum = -1;  // unused
        packet->seqnum = seqnum;
        memcpy(packet->payload, message.data, sizeof(message.data));
        packet->checksum = pUtils->calculateCheckSum(*packet);
        return packet;
    }

    std::shared_ptr<Packet> make_ack_packet(const Packet& rcv_packet) {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        packet->seqnum = -1;  // unused
        packet->acknum = rcv_packet.seqnum;
        for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++){
		    packet->payload[i] = '.';
	    }
        packet->checksum = pUtils->calculateCheckSum(*packet);
        return packet;
    }

    bool no_corrupt(const Packet& packet) {
        return packet.checksum == pUtils->calculateCheckSum(packet);
    }

    Message extract(const Packet& packet) {
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        return msg;
    }

    bool stop_in_message(const Message& msg, char msg_first_char) {
        std::string tmp = msg.data;
        return tmp[0] == msg_first_char;
    }

}  // Helpers