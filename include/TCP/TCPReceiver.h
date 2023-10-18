#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

#include "GBN/GBNRdtReceiver.h"

class TCPReceiver : public GBNRdtReceiver {
public:
    TCPReceiver();
    ~TCPReceiver();

};

#endif