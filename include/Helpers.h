// class RangeChecker {
// public:
//     static bool check_in_window(int target, int base);
//     static bool check_in_range(int target, int lb, int ub);
//     static void init(int window_size, int max_seqnum);

// private:
//     static int _window_size;
//     static int _max_seqnum;

//     RangeChecker() = default;
//     ~RangeChecker() = default;
// };

#include "DataStructure.h"
#include <memory>

namespace Helpers {

    bool check_in_window(int target, int base);
    bool check_in_range(int target, int lb, int ub);
    void init(int window_size, int max_seqnum);

    std::shared_ptr<Packet> make_packet(const Message& message, int seqnum);

};