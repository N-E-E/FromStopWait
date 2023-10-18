#include "DataStructure.h"
#include "Global.h"
#include "Helpers.h"
#include "Config.h"
#include "GBNRdtSender.h"
#include "RandomEventEnum.h"

#include <cmath>


GBNRdtSender::GBNRdtSender() :   _k(SEQNUM_BIT), _N(WINDOW_SIZE), _max_seqnum(MAX_SEQNUM), 
                                _waiting_state(false), _base(0), _next_seqnum(0)
{
    _packet_waiting_ack.resize(_max_seqnum);
    logger->print_window(_base, _next_seqnum);
}

GBNRdtSender::GBNRdtSender(int k, int N) : _base(0), _next_seqnum(0), _waiting_state(false), 
                                _k(k), _N(N), _max_seqnum(pow(2, _k))
{
    _packet_waiting_ack.resize(_max_seqnum);
}

GBNRdtSender::~GBNRdtSender() {}

bool GBNRdtSender::send(const Message &message) {
    if (_waiting_state == true) {
        // window is full, refuse data
        // std::cout << "packet is blocking..." << std::endl;
        logger->common_info("packet is blocking...");
        logger->print_window(_base, _next_seqnum);
        return false;
    }

    if (Helpers::check_in_window(_next_seqnum, _base)) {
        // send packet and start timer
        _packet_waiting_ack[_next_seqnum] = Helpers::make_msg_packet(message, _next_seqnum);
#ifdef DEBUG_STOP_IN_MSG
        if (Helpers::stop_in_message(message, 'E')) {
            int a = 1;
        }
#endif
        // pUtils->printPacket("发送方发送报文", *_packet_waiting_ack[_next_seqnum]);
        logger->print_packet("发送方发送报文", *_packet_waiting_ack[_next_seqnum]);
        pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[_next_seqnum]);
        if (_base == _next_seqnum) {
            pns->startTimer(SENDER, Configuration::TIME_OUT, _base);
        }

        // update info
        _next_seqnum = (_next_seqnum + 1) % _max_seqnum;
        if (Helpers::check_in_window(_next_seqnum, _base)) {  // NOTES: must update here! if go down, 
                                                                            //A packet will be lost under the test frame!
            _waiting_state = false;
        } else {
            _waiting_state = true;
        }
        logger->print_window(_base, _next_seqnum);
        return true;
    }
    // abnormal case
    throw "An error occur in GBNRdtSender::send";
    return false;
    
}

void GBNRdtSender::receive(const Packet& ackPacket) {
    if (_base == _next_seqnum) {  // force to forbid receive if nothing is sent.
        return;
    }
    _waiting_state = false;         //NOTES: must add one here. or the test frame will be stuck
                                    //(if receive don't change the state, then send will be stuck)
    if (Helpers::no_corrupt(ackPacket) && 
        Helpers::check_in_range(ackPacket.acknum, _base, (_base + _N) % _max_seqnum)) {  // TODO: here?
        
        // pUtils->printPacket("发送方正确收到确认", ackPacket);
        logger->print_packet("发送方正确收到确认", ackPacket);
        
        // update info
        int latest_base = (ackPacket.acknum + 1) % _max_seqnum;
        if (latest_base == _next_seqnum) {
            pns->stopTimer(SENDER, _base);
        } else {
            // (restart timer for the latest base)stop and then start
            pns->stopTimer(SENDER, _base);
            pns->startTimer(SENDER, Configuration::TIME_OUT, latest_base);
        }
        _base = latest_base;  // update base after update timer.

    } else if ( !Helpers::no_corrupt(ackPacket) ) {  // ackPacket corrupt
        // TODO: whether to resend now ?
        int upper_bound = _base > _next_seqnum ? _next_seqnum + _max_seqnum : _next_seqnum;  // be carefully!
        for (auto i = _base; i < upper_bound; i++) {
            int i_in_loop = i % _max_seqnum;
            std::string hint = "发送方没有正确收到确认，重发上次发送的报文-" + std::to_string(i_in_loop);
            // pUtils->printPacket(hint.c_str(), *_packet_waiting_ack[i_in_loop]);
            logger->print_packet(hint.c_str(), *_packet_waiting_ack[i_in_loop]);
            pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[i_in_loop]);
        }

        // (restart timer)stop and then start
        pns->stopTimer(SENDER, _base);
        pns->startTimer(SENDER, Configuration::TIME_OUT, _base);
        
    } else {  // ackPacket.acknum < _base
        // do nothing and waiting for resend.
    }

    logger->print_window(_base, _next_seqnum);
    return;
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    // assert(seqNum == _base);  // for test.
    int upper_bound = _base > _next_seqnum ? _next_seqnum + _max_seqnum : _next_seqnum;  // be carefully!
    for (auto i = _base; i < upper_bound; i++) {
        int i_in_loop = i % _max_seqnum;
        std::string hint = "发送方定时器时间到，重发上次发送的报文-" + std::to_string(i_in_loop);
        // pUtils->printPacket(hint.c_str(), *_packet_waiting_ack[i_in_loop]);
        logger->print_packet(hint.c_str(), *_packet_waiting_ack[i_in_loop]);
        pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[i_in_loop]);
    }
    // restart timer
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
    // assert(0);  // for test.
}
