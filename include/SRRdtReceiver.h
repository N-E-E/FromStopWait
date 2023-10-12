#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include "RdtReceiver.h"

#include <vector>

class SRRdtReceiver : public RdtReceiver {
public:
    void receive(const Packet& packet);

    bool no_corrupt(const Packet& packet);

    Message extract(const Packet& packet);

    void make_packet(const Packet& rcv_packet);

    bool check_seqnum_in_cur_range(int seqnum);

    bool check_num_in_loop_range(int num, int lb, int ub, int t);

    SRRdtReceiver();
    ~SRRdtReceiver();
    

private:
    int _k, _max_seqnum;
    int _N;
    int _rcv_base;
    Packet _last_ack_packet;
    std::vector<Packet> _rcv_packet;
    std::vector<bool> _rcv_state;
};


#endif