#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H

#include "RdtSender.h"

#include <memory>
#include <vector>
#include <cmath>

class SRRdtSender : public RdtSender {
public:
    bool getWaitingState() { return _waiting_state; }
	bool send(const Message &message);  // 发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet &ackPkt);  // 接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);  // Timeout handler，将被NetworkServiceSimulator调用

    SRRdtSender();
    ~SRRdtSender();

private:
    int _k, _max_seqnum;
    int _N;
    int _send_base, _next_seqnum;
    bool _waiting_state;
    std::vector<std::shared_ptr<Packet>> _packet_waiting_ack;  // TODO: change it to smart ptr
    std::vector<bool> _ack_state;
};

#endif