//
// Created by ahmed on 12/25/16.
//

#ifndef ENHANCEDUDPCLIENT_GBNRECEIVER_H
#define ENHANCEDUDPCLIENT_GBNRECEIVER_H


#include <queue>
#include "MainSocket.h"
#include "client_config.h"
#include "FileWriter.h"
#include "AbstractReceiver.h"
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>

// TODO GBN receiver and FileTransfer classes have overlapping functionality
// which needs to be eliminated

class GbnReceiver : public AbstractReceiver
{
    unsigned int window_size;

    // A pointer must be used, as the lockfree queue needs
    // types with simple destructor and copy constructor
    // which aren't valid for the unique pointer
    boost::lockfree::queue<Packet *> packets;

    void Receive();

    void StartAcking();

public:

    GbnReceiver(unsigned int window_size,
            unique_ptr<RawUdpSocket> &sock,
            unique_ptr<FileWriter> &writer, AddressInfo &server_info);

    void StartReceiving() override;

    void StopReceiving() override;

};


#endif //ENHANCEDUDPCLIENT_GBNRECEIVER_H
