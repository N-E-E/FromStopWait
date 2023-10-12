#include "SRRdtSender.h"
#include "RandomEventEnum.h"
#include "DataStructure.h"
#include "Global.h"

SRRdtSender::SRRdtSender() : _k(3), _max_seqnum(pow(2, _k)), _N(4),
                            _send_base(0), _next_seqnum(0), _waiting_state(false) 
{
    _packet_waiting_ack.resize(_max_seqnum);
    _ack_state.resize(_max_seqnum);
}

SRRdtSender::~SRRdtSender(){}

bool SRRdtSender::send(const Message& message) {
    if (message.data[0] == 'F') {
        std::cout << "here" << std::endl;
    }
    if (_waiting_state) {
        return false;
    }
    if (check_next_seqnum_in_window(_next_seqnum, _send_base)) {
        // send packet and start timer
        make_packet(_packet_waiting_ack[_next_seqnum], message, _next_seqnum);
        pUtils->printPacket("发送方发送报文", _packet_waiting_ack[_next_seqnum]);
        pns->sendToNetworkLayer(RECEIVER, _packet_waiting_ack[_next_seqnum]);
        pns->startTimer(SENDER, Configuration::TIME_OUT, _next_seqnum);

        // update info
        _ack_state[_next_seqnum] = false;
        _next_seqnum = (_next_seqnum + 1) % _max_seqnum;
        if (check_next_seqnum_in_window(_next_seqnum, _send_base)) {  // NOTES: must update here! if go down, 
                                                                                            //A packet will be lost under the test frame!
            _waiting_state = false;
        } else {
            _waiting_state = true;
        }
        return true;
    }
    // window is full, refuse data
    _waiting_state = true;
    std::cout << "packet is blocking..." << std::endl;
    return false;
}

void SRRdtSender::receive(const Packet &ackPacket) {
    _waiting_state = false;  //NOTES: must add one here. or the test frame will be stuck
                            //(if receive don't change the state, then send will be stuck)
    // assert(ackPkt.acknum < _next_seqnum);
    if (check_acknum_in_range(ackPacket.acknum) && no_corrupt(ackPacket)) {  // actually is else.
        pUtils->printPacket("发送方正确收到确认", ackPacket);
        pns->stopTimer(SENDER, ackPacket.acknum);
        _ack_state[ackPacket.acknum] = true;
        // check if _send_base can move forward
        while (_ack_state[_send_base] == true && _send_base != _next_seqnum) {
            _send_base = (_send_base + 1) % _max_seqnum;
            std::cout << "_send_base move to " << _send_base << std::endl;
        }
    } else {
        // do nothing
        // if ack is corrupt: resend when count down.
        return;
    }
}

void SRRdtSender::timeoutHandler(int seqNum) {
    std::string hint = "发送方定时器时间到，重发上次发送的报文-" + std::to_string(seqNum);
    pUtils->printPacket(hint.c_str(), _packet_waiting_ack[seqNum]);
    pns->sendToNetworkLayer(RECEIVER, _packet_waiting_ack[seqNum]);
    // restart timer
    pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}


void SRRdtSender::make_packet(Packet& packet, const Message& message, int seqnum) {
    packet.acknum = -1;  // unused
    packet.seqnum = seqnum;
    memcpy(packet.payload, message.data, sizeof(message.data));
    packet.checksum = pUtils->calculateCheckSum(packet);
}

bool SRRdtSender::check_next_seqnum_in_window(int next_seqnum, int send_base) {
    if (next_seqnum < send_base) {
        return next_seqnum < (send_base + _N) % _max_seqnum;
    } else {
        return next_seqnum < send_base + _N;
    }
}

bool SRRdtSender::check_acknum_in_range(int acknum) {
    if (_send_base <= _next_seqnum) {
        return acknum >= _send_base && acknum < _next_seqnum;
    } else {
        if (acknum >= _send_base) return acknum < _next_seqnum + _max_seqnum;
        else return acknum < _next_seqnum;
    }
    return false;
}

// bool SRRdtSender::check_acknum_valid(int acknum) {
//     if (acknum > _next_seqnum) {
//         return 
//     }
// }

bool SRRdtSender::no_corrupt(const Packet& packet) {
    return packet.checksum == pUtils->calculateCheckSum(packet);
}