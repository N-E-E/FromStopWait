#include "Global.h"
#include "GBNRdtReceiver.h"
#include "RandomEventEnum.h"

#include <cstring>
#include <cmath>
#include <cassert>

GBNRdtReceiver::GBNRdtReceiver() : _expected_seqnum(0), _k(3), _max_seqnum(pow(2, _k)) {
    _last_ack_packet.seqnum = -1;  // unused
    _last_ack_packet.acknum = -1;  // init
    for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++){
		_last_ack_packet.payload[i] = '.';
	}
    _last_ack_packet.checksum = pUtils->calculateCheckSum(_last_ack_packet);
}

GBNRdtReceiver::~GBNRdtReceiver() {}

void GBNRdtReceiver::receive(const Packet& packet) {
    if (no_corrupt(packet) && _expected_seqnum == packet.seqnum) {
        pUtils->printPacket("接收方正确收到发送方的报文", packet);
        // extract & deliver
        pns->delivertoAppLayer(RECEIVER, extract(packet));
        // send ack
        make_packet(packet);
        pUtils->printPacket("接收方发送确认报文", _last_ack_packet);
        pns->sendToNetworkLayer(SENDER, _last_ack_packet);
        // update seqnum
        _expected_seqnum = (_expected_seqnum + 1) % _max_seqnum;
    } else {
        if (!no_corrupt(packet)) {
            pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        } else {
            pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
            std::cout << "expect=" << _expected_seqnum << std::endl;
            // assert(0);
        }
        pUtils->printPacket("接收方重新发送上次的确认报文", _last_ack_packet);
        pns->sendToNetworkLayer(SENDER, _last_ack_packet);
    }
    // assert(0);  // for test
}


void GBNRdtReceiver::make_packet(const Packet& rcv_packet) {
    _last_ack_packet.seqnum = -1;  // unused
    _last_ack_packet.acknum = rcv_packet.seqnum;
    _last_ack_packet.checksum = pUtils->calculateCheckSum(_last_ack_packet);
}

bool GBNRdtReceiver::no_corrupt(const Packet& packet) {
    return packet.checksum == pUtils->calculateCheckSum(packet);
}

Message GBNRdtReceiver::extract(const Packet& packet) {  // FIXME: return a ref to reduce copy in return
    Message msg;
    memcpy(msg.data, packet.payload, sizeof(packet.payload));
    return msg;
}