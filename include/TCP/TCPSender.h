#ifndef TCP_SENDER_H
#define TCP_SENDER_H

#include "GBN/GBNRdtSender.h"

class TCPSender : public GBNRdtSender {
public:
    TCPSender();
    ~TCPSender();

    void receive(const Packet& ackPacket) override;

private:
    int _final_ack;
    int _dup_ack_count;

};

#endif