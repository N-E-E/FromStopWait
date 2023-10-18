#include "TCP/TCPSender.h"
#include "Global.h"
#include "Helpers.h"

TCPSender::TCPSender() : GBNRdtSender(), _dup_ack_count(0), _final_ack(-1) { }

TCPSender::~TCPSender() { }

void TCPSender::receive(const Packet& ackPacket) {
    if (_base == _next_seqnum) {  // force to forbid receive if nothing is sent.
        return;
    }
    _waiting_state = false;         //NOTES: must add one here. or the test frame will be stuck
                                    //(if receive don't change the state, then send will be stuck)
    if (Helpers::no_corrupt(ackPacket) && 
        Helpers::check_in_range(ackPacket.acknum, _base, (_base + _N) % _max_seqnum)) {  // TODO: here?
        
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
        _final_ack = ackPacket.acknum;  // record final ack

    } else if ( !Helpers::no_corrupt(ackPacket) ) {  // ackPacket corrupt
        // TODO: whether to resend now ?
        int upper_bound = _base > _next_seqnum ? _next_seqnum + _max_seqnum : _next_seqnum;  // be carefully!
        for (auto i = _base; i < upper_bound; i++) {
            int i_in_loop = i % _max_seqnum;
            std::string hint = "发送方没有正确收到确认，重发上次发送的报文-" + std::to_string(i_in_loop);
            logger->print_packet(hint.c_str(), *_packet_waiting_ack[i_in_loop]);
            pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[i_in_loop]);
        }

        // (restart timer)stop and then start
        pns->stopTimer(SENDER, _base);
        pns->startTimer(SENDER, Configuration::TIME_OUT, _base);
        
    } else {  // ackPacket.acknum < _base
        // do nothing and waiting for resend --> change it to fast resend.
        if (ackPacket.acknum == _final_ack) {
            _dup_ack_count++;
        } else {
            _dup_ack_count = 0;
        }
        
        if (_dup_ack_count == 3) {
            _dup_ack_count = 0;
            logger->print_packet("发送方快速重传报文", *_packet_waiting_ack[_final_ack]);
            pns->sendToNetworkLayer(RECEIVER, *_packet_waiting_ack[_final_ack]);

            // (restart timer)stop and then start
            pns->stopTimer(SENDER, _base);
            pns->startTimer(SENDER, Configuration::TIME_OUT, _base);
        }
    }

    logger->print_window(_base, _next_seqnum, "send-window");
    return;
}