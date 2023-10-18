#include "Config.h"
#include "Global.h"
#include "GBNRdtReceiver.h"
#include "Helpers.h"
#include "RandomEventEnum.h"

#include <cstring>
#include <cmath>
#include <cassert>
#include <string>

GBNRdtReceiver::GBNRdtReceiver() : _expected_seqnum(0), _k(WINDOW_SIZE), _max_seqnum(MAX_SEQNUM) {
    _last_ack_packet = std::make_shared<Packet>();
    _last_ack_packet->seqnum = -1;  // unused
    _last_ack_packet->acknum = -1;  // init
    for(int i = 0; i < Configuration::PAYLOAD_SIZE; i++){
		_last_ack_packet->payload[i] = '.';
	}
    _last_ack_packet->checksum = pUtils->calculateCheckSum(*_last_ack_packet);
}

GBNRdtReceiver::~GBNRdtReceiver() {}

void GBNRdtReceiver::receive(const Packet& packet) {
    if (Helpers::no_corrupt(packet) && _expected_seqnum == packet.seqnum) {
        // pUtils->printPacket("接收方正确收到发送方的报文", packet);
        logger->print_packet("接收方正确收到发送方的报文", packet);
        // extract & deliver
        pns->delivertoAppLayer(RECEIVER, Helpers::extract(packet));
        // send ack
        _last_ack_packet = Helpers::make_ack_packet(packet);
        // pUtils->printPacket("接收方发送确认报文", *_last_ack_packet);
        logger->print_packet("接收方发送确认报文", *_last_ack_packet);
        pns->sendToNetworkLayer(SENDER, *_last_ack_packet);
        // update seqnum
        _expected_seqnum = (_expected_seqnum + 1) % _max_seqnum;

    } else {
        if (!Helpers::no_corrupt(packet)) {
            // pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
            logger->print_packet("接收方没有正确收到发送方的报文,数据校验错误", packet);
        } else {
            // pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
            logger->print_packet("接收方没有正确收到发送方的报文,报文序号不对", packet);
            // std::cout << "expect seqnum = " << _expected_seqnum << std::endl;
            logger->common_info("expect seqnum = " + std::to_string(_expected_seqnum));
            // assert(0);
        }
        // pUtils->printPacket("接收方重新发送上次的确认报文", *_last_ack_packet);
        logger->print_packet("接收方重新发送上次的确认报文", *_last_ack_packet);
        pns->sendToNetworkLayer(SENDER, *_last_ack_packet);
    }
    // assert(0);  // for test
}
