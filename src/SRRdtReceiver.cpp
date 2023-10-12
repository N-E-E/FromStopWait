#include "Global.h"
#include "Config.h"
#include "Helpers.h"
#include "RandomEventEnum.h"
#include "SRRdtReceiver.h"

#include <cmath>
#include <memory>
#include <vector>

SRRdtReceiver::SRRdtReceiver() : _k(SEQNUM_BIT), _max_seqnum(MAX_SEQNUM), _N(WINDOW_SIZE),
                                _rcv_base(0)
{
    _rcv_packet.resize(_max_seqnum);
    _rcv_state = std::vector<bool>(_max_seqnum, false);

    _last_ack_packet = std::make_shared<Packet>();
    _last_ack_packet->seqnum = -1;  // unused
    _last_ack_packet->acknum = -1;  // init
    for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++){
		_last_ack_packet->payload[i] = '.';
	}
    _last_ack_packet->checksum = pUtils->calculateCheckSum(*_last_ack_packet);
}

SRRdtReceiver::~SRRdtReceiver() {}

void SRRdtReceiver::receive(const Packet& packet) {
    if (Helpers::no_corrupt(packet)) {
        if (Helpers::check_in_range(packet.seqnum, _rcv_base, (_rcv_base + _N) % _max_seqnum)) {
            // rcv the packet
            pUtils->printPacket("接收方正确收到发送方的报文", packet);
            _rcv_packet[packet.seqnum] = std::make_shared<Packet>(packet);
            _rcv_state[packet.seqnum] = true;

            // send ack
            _last_ack_packet = Helpers::make_ack_packet(packet);
            pUtils->printPacket("接收方发送确认报文", *_last_ack_packet);
            pns->sendToNetworkLayer(SENDER, *_last_ack_packet);

            // check if some packets can be deliver up
            int upper_bound = _rcv_base + _N;
            while (_rcv_state[_rcv_base] == true && _rcv_base != upper_bound) {
                std::cout << "deliver packet " << _rcv_base << std::endl;
                pns->delivertoAppLayer(RECEIVER, Helpers::extract(*_rcv_packet[_rcv_base]));
                // update info
                _rcv_state[_rcv_base] = false;
                _rcv_base = (_rcv_base + 1) % _max_seqnum;
            }
        } else {  // FIXEME: only fit current N = max_seqnum / 2. but how to judge as the ppt?
            // resend ack.
            _last_ack_packet = Helpers::make_ack_packet(packet);
            pUtils->printPacket("接收到已确认区间的数据，接收方重新发送发送确认报文", *_last_ack_packet);
            pns->sendToNetworkLayer(SENDER, *_last_ack_packet);
        } 
    } else {
        pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        // do nothing and just wait for the timer to restart?
        return;
    }
}

