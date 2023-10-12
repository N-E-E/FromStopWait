#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include "RdtReceiver.h"

#include <memory>
#include <vector>

class SRRdtReceiver : public RdtReceiver {
public:
    void receive(const Packet& packet);

    SRRdtReceiver();
    ~SRRdtReceiver();
    

private:
    int _k, _max_seqnum;
    int _N;
    int _rcv_base;
    std::shared_ptr<Packet> _last_ack_packet;
    std::vector<std::shared_ptr<Packet>> _rcv_packet;
    std::vector<bool> _rcv_state;
};


#endif