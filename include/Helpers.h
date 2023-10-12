#ifndef HELPERS_H
#define HELPERS_H

#include "DataStructure.h"
#include <memory>

namespace Helpers {

    bool check_in_window(int target, int base);
    bool check_in_range(int target, int lb, int ub);
    void init(int window_size, int max_seqnum);

    std::shared_ptr<Packet> make_msg_packet(const Message& message, int seqnum);
    std::shared_ptr<Packet> make_ack_packet(const Packet& rcv_packet);
    bool no_corrupt(const Packet& packet);
    Message extract(const Packet& packet);

    bool stop_in_message(const Message& msg, char msg_first_char);
};

#endif