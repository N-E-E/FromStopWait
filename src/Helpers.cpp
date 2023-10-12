#include "Helpers.h"
#include "DataStructure.h"
#include "Global.h"

#include <cstring>
#include <memory>

// int RangeChecker::_window_size = 0;
// int RangeChecker::_max_seqnum = 0;

// bool RangeChecker::check_in_range(int target, int lb, int ub) {
//     if (lb <= ub) {
//         return target >= lb && target < ub;
//     } else {
//         if (target >= lb) return target < ub + _max_seqnum;
//         else return target < ub;
//     }
//     return false;
// }

// bool RangeChecker::check_in_window(int target, int base) {
//     if (target < base) {
//         return target < (base + _window_size) % _max_seqnum;
//     } else {
//         return target < base + _window_size;
//     }
// }

// void RangeChecker::init(int window_size, int max_seqnum) {
//     _window_size = window_size;
//     _max_seqnum = max_seqnum;
// }

namespace Helpers {
    int window_size_ = 0, max_seqnum_ = 0;

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

    std::shared_ptr<Packet> make_packet(const Message& message, int seqnum) {
        std::shared_ptr<Packet> packet = std::make_shared<Packet>();
        packet->acknum = -1;  // unused
        packet->seqnum = seqnum;
        memcpy(packet->payload, message.data, sizeof(message.data));
        packet->checksum = pUtils->calculateCheckSum(*packet);
        return packet;
    }

}  // Helpers