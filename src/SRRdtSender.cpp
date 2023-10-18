#include "SRRdtSender.h"
#include "RandomEventEnum.h"
#include "DataStructure.h"
#include "Global.h"
#include "Helpers.h"
#include "Config.h"
#include <string>

SRRdtSender::SRRdtSender() : _k(SEQNUM_BIT), _max_seqnum(MAX_SEQNUM), _N(WINDOW_SIZE),
                            _send_base(0), _next_seqnum(0), _waiting_state(false) 
{
    _packet_waiting_ack.resize(_max_seqnum);
    _ack_state.resize(_max_seqnum);
    logger->print_window(_send_base, _next_seqnum, "send-window");
}

SRRdtSender::~SRRdtSender(){}

bool SRRdtSender::send(const Message& message) {
#ifdef DEBUG_STOP_IN_MSG
    if (Helpers::stop_in_message(message, 'F')) {
        int a = 1;
    }
#endif
    if (_waiting_state) {
        // window is full, refuse data
        logger->common_info("packet is blocking...");
        logger->print_window(_send_base, _next_seqnum, "send-window");
        return false;
    }
    if (Helpers::check_in_window(_next_seqnum, _send_base)) {
        // send packet and start timer
        _packet_waiting_ack[_next_seqnum] = Helpers::make_msg_packet(message, _next_seqnum);
        logger->print_packet("发送方发送报文", *_packet_waiting_ack[_next_seqnum]);
        pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[_next_seqnum]);
        pns->startTimer(SENDER, Configuration::TIME_OUT, _next_seqnum);

        // update info
        _ack_state[_next_seqnum] = false;
        _next_seqnum = (_next_seqnum + 1) % _max_seqnum;
        if (Helpers::check_in_window(_next_seqnum, _send_base)) {  // NOTES: must update here! if go down, 
                                                                                // A packet will be lost under the test frame!
            _waiting_state = false;
        } else {
            _waiting_state = true;
        }

        logger->print_window(_send_base, _next_seqnum, "send-window");
        return true;
    }
    // abnormal case
    throw "An error occur in SRRdtSender::send";
    return false;
}

void SRRdtSender::receive(const Packet &ackPacket) {
    _waiting_state = false;  // NOTES: must add one here. or the test frame will be stuck
                            // (if receive don't change the state, then send will be stuck and receive forever)
    // assert(ackPkt.acknum < _next_seqnum);
    if (Helpers::no_corrupt(ackPacket) && 
        Helpers::check_in_range(ackPacket.acknum, _send_base, _next_seqnum)) {
        
        logger->print_packet("发送方正确收到确认", ackPacket);
        pns->stopTimer(SENDER, ackPacket.acknum);
        _ack_state[ackPacket.acknum] = true;
        // check if _send_base can move forward
        while (_ack_state[_send_base] == true && _send_base != _next_seqnum) {
            _send_base = (_send_base + 1) % _max_seqnum;
            logger->common_info("_send_base move to " + std::to_string(_send_base));
        }
    } else {
        // do nothing
        // if ack is corrupt: resend when count down.
    }
    
    logger->print_window(_send_base, _next_seqnum, "send-window");
    return;
}

void SRRdtSender::timeoutHandler(int seqNum) {
    std::string hint = "发送方定时器时间到，重发上次发送的报文-" + std::to_string(seqNum);
    logger->print_packet(hint.c_str(), *_packet_waiting_ack[seqNum]);
    pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[seqNum]);
    // restart timer
    pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}
