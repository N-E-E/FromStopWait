#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H

#include "RdtSender.h"
#include <vector>
#include <cmath>

class GBNRdtSender : public RdtSender {
private:
    int _base;
    int _next_seqnum;
    int _N, _k, _max_seqnum;
    bool _waiting_state;
    std::vector<Packet> _packet_waiting_ack;  // TODO: change the member to smart ptr

public:
    bool getWaitingState() { return _waiting_state; }
	bool send(const Message &message);  // 发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet &ackPkt);  // 接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);  // Timeout handler，将被NetworkServiceSimulator调用

    void make_packet(Packet& packet, const Message& message, int seqnum);
    bool no_corrupt(const Packet& packet);
    void resend_group();
public:
    GBNRdtSender();
    GBNRdtSender(int k, int N);
    ~GBNRdtSender();  // TODO: use virtual? why?

};

#endif