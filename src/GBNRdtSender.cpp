#include "DataStructure.h"
#include "Global.h"
#include "GBNRdtSender.h"
#include "RandomEventEnum.h"


GBNRdtSender::GBNRdtSender() : _base(0), _next_seqnum(0), _waiting_state(false), 
                                _k(3), _N(4), _max_seqnum(pow(2, _k)) 
{
    _packet_waiting_ack.resize(_max_seqnum);
}

GBNRdtSender::GBNRdtSender(int k, int N) : _base(0), _next_seqnum(0), _waiting_state(false), 
                                _k(k), _N(N), _max_seqnum(pow(2, _k))
{
    _packet_waiting_ack.resize(_max_seqnum);
}

GBNRdtSender::~GBNRdtSender() {}

bool GBNRdtSender::send(const Message &message) {
    std::cout << "enter send" << std::endl;
    if (_waiting_state == true) {  
        return false;
    }

    if (check_next_seqnum_in_window(_next_seqnum, _base)) {
        // send packet and start timer
        make_packet(_packet_waiting_ack[_next_seqnum], message, _next_seqnum);
        // for debug
        std::string tmp = message.data;
        // std::cout << tmp.length() << std::endl;
        if (tmp[0] == 'E') {
            int a = 1;
            // std::cout << tmp << std::endl;
            // std::cout << tmp.size() << std::endl;
            // std::cout << "GGGGGGGGGGGGGGGGGGGG" << std::endl;
        }
        pUtils->printPacket("发送方发送报文", _packet_waiting_ack[_next_seqnum]);
        pns->sendToNetworkLayer(RECEIVER, _packet_waiting_ack[_next_seqnum]);
        if (_base == _next_seqnum) {
            pns->startTimer(SENDER, Configuration::TIME_OUT, _base);
        }

        // update info
        _next_seqnum = (_next_seqnum + 1) % _max_seqnum;
        if (check_next_seqnum_in_window(_next_seqnum, _base)) {  // NOTES: must update here! if go down, 
                                                                                            //A packet will be lost under the test frame!
            _waiting_state = false;
        } else {
            _waiting_state = true;
        }
        return true;
    }
    // TODO: refuse data
    this->_waiting_state = true;
    std::cout << "packet is blocking..." << std::endl;
    return false;
    
}

void GBNRdtSender::receive(const Packet& ackPacket) {
    std::cout << "enter receive" << std::endl;
    if (_base == _next_seqnum) {  // force to forbid receive if nothing is sent.
        return;
    }
    _waiting_state = false;         //NOTES: must add one here. or the test frame will be stuck
                                    //(if receive don't change the state, then send will be stuck)
    if (no_corrupt(ackPacket) && 
        check_num_in_loop_range(ackPacket.acknum, _base, (_base + _N) % _max_seqnum, _max_seqnum)) {  // TODO: here?
        
        pUtils->printPacket("发送方正确收到确认", ackPacket);
        // this->_waiting_state = false;
        int latest_base = (ackPacket.acknum + 1) % _max_seqnum;
        if (latest_base == _next_seqnum) {
            pns->stopTimer(SENDER, _base);
        } else {
            // (restart timer for the latest base)stop and then start
            pns->stopTimer(SENDER, _base);
            pns->startTimer(SENDER, Configuration::TIME_OUT, latest_base);
        }
        _base = latest_base;  // update base after update timer.
    } else if (!no_corrupt(ackPacket)) {  // ackPacket corrupt
        // TODO: whether to resend now ?
        int upper_bound = _base > _next_seqnum ? _next_seqnum + _max_seqnum : _next_seqnum;  // be carefully!
        for (auto i = _base; i < upper_bound; i++) {
            int i_in_loop = i % _max_seqnum;
            std::string hint = "发送方没有正确收到确认，重发上次发送的报文-" + std::to_string(i_in_loop);
            pUtils->printPacket(hint.c_str(), _packet_waiting_ack[i_in_loop]);
            pns->sendToNetworkLayer(RECEIVER, _packet_waiting_ack[i_in_loop]);
        }
        // (restart timer)stop and then start
        pns->stopTimer(SENDER, _base);
        pns->startTimer(SENDER, Configuration::TIME_OUT, _base);
    } else {  // ackPacket.acknum < _base
        // do nothing.
    }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    // assert(seqNum == _base);  // for test.
    std::cout << "enter timeoutHandler" << std::endl;
    int upper_bound = _base > _next_seqnum ? _next_seqnum + _max_seqnum : _next_seqnum;  // be carefully!
    for (auto i = _base; i < upper_bound; i++) {
        int i_in_loop = i % _max_seqnum;
        std::string hint = "发送方定时器时间到，重发上次发送的报文-" + std::to_string(i_in_loop);
        pUtils->printPacket(hint.c_str(), _packet_waiting_ack[i_in_loop]);
        pns->sendToNetworkLayer(RECEIVER, _packet_waiting_ack[i_in_loop]);
    }
    // restart timer
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
    // assert(0);  // for test.
}



void GBNRdtSender::make_packet(Packet& packet, const Message& message, int seqnum){
    packet.acknum = -1;  // unused
    packet.seqnum = seqnum;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);
}

bool GBNRdtSender::no_corrupt(const Packet& packet) {
    return packet.checksum == pUtils->calculateCheckSum(packet);
}

bool GBNRdtSender::check_next_seqnum_in_window(int next_seqnum, int base) {
    if (next_seqnum < base) {
        return next_seqnum < (base + _N) % _max_seqnum;
    } else {
        return next_seqnum < base + _N;
    }
}

bool GBNRdtSender::check_num_in_loop_range(int num, int lb, int ub, int t) {
    if (lb <= ub) {
        return num >= lb && num < ub;
    } else {
        if (num >= lb) return num < ub + t;
        else return num < ub;
    }
    return false;
}
