//
// Created by ahmed on 12/19/16.
//

#ifndef ENHANCEDUDPCLIENT_ACKPACKETS_H
#define ENHANCEDUDPCLIENT_ACKPACKETS_H
struct ack_packet
{
    unsigned short cksum;
    unsigned short len;
    unsigned intackno;

};
#endif //ENHANCEDUDPCLIENT_ACKPACKETS_H
