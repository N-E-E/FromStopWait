#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H

#include "RdtReceiver.h"
#include <memory>

class GBNRdtReceiver : public RdtReceiver {
public:
    GBNRdtReceiver();
    virtual ~GBNRdtReceiver();

    void receive(const Packet& packet);

    void make_packet(const Packet& rcv_packet);

    bool no_corrupt(const Packet& packet);

    Message extract(const Packet& packet);

private:
    int _expected_seqnum;
    int _k, _max_seqnum;
    std::shared_ptr<Packet> _last_ack_packet;
};


#endif