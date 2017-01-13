//
// Created by ahmed on 12/25/16.
//

// TODO create a base class for the GBN and Selective Repeat receivers
// TODO use the timers
#include "GbnReceiver.h"

GbnReceiver::GbnReceiver(unsigned int window_size,
        unique_ptr <RawUdpSocket> &sock,
        unique_ptr <FileWriter> &f_writer,
        AddressInfo &server_info) :
        packets(window_size),
        AbstractReceiver(server_info, f_writer, sock)
{
    this->window_size = window_size;
}

void GbnReceiver::StopReceiving()
{

}

void GbnReceiver::StartReceiving()
{
    // TODO, make the start receiving function call another function with a thread creation,
    // so the user just calls start receiving without having to worry about threads

    boost::thread rcv_th(boost::bind(&GbnReceiver::Receive, boost::ref(*this)));
    boost::thread ack_th(boost::bind(&GbnReceiver::StartAcking, boost::ref(*this)));

    rcv_th.join();
    ack_th.join();
}

void GbnReceiver::StartAcking()
{
    while (this->is_receiving) {

//        if (boost::this_thread::interruption_requested()) {
//            // TODO cleanup and pause AKCing
//        }

        // Wait for packets to be sent and print out
        if (this->packets.empty()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
            continue;
        }

        Packet *temp;
        if (!this->packets.pop(temp)) continue;  // queue is empty  // TODO remove this, redundant

        unique_ptr <Packet> to_be_acked(temp);   // Put the packet in a container

        // The popped packet isn't the one I'm waiting for
        if (to_be_acked->header->seqno != (this->last_acked_seq_num + 1)) {
            cerr << "#-- | Bad SEQ [" << to_be_acked->header->seqno << "]" << endl;
            continue; // Do nothing, the server will timeout on its own
        }

        cout << "--> | ACK [" << to_be_acked->header->seqno << "]" << endl;
        unique_ptr <ByteVector> data = nullptr;
        unique_ptr <Packet> ack(new Packet(data, to_be_acked->header->seqno));
        file_transfer_socket->SendPacket(this->server_info, ack);

        this->last_acked_seq_num++;

        bool is_final_ack = to_be_acked->header->dataLen == 0;
        if (!is_final_ack && to_be_acked->GetData(data)) {
            //cout << "Data recv.:" << data->data() << endl;
            this->writer->Write(*data);

        } else if (is_final_ack) {
            cout << "Transmission completed at packet [" << to_be_acked->header->seqno << "]" << endl;
            this->is_receiving = false;
            this->writer->Close();
            return;
        } else {
            cerr << "Corrupt packet" << endl;
        }
    }

}

void GbnReceiver::Receive()
{

    int max_fails_of_packet = 9;
    int fails_of_packet = 0;

    this->is_receiving = true;

    while (this->is_receiving) {

//        if (boost::this_thread::interruption_requested()) {
//            // TODO cleanup and pause AKCing
//        }


        unique_ptr <Packet> unq_pckt;
        if (file_transfer_socket->ReceivePacket(unq_pckt)) {
            // Reset fail count on valid packet, this isn't reached on timeout
            fails_of_packet = 0;
            cout << "<-- | RCV [" << unq_pckt->header->seqno << "]" << endl;
            this->packets.push(unq_pckt.release());
        } else {
            if (!this->is_receiving) return;

            cerr << "-*- | Timeout" << endl;

            // TODO terminate of many fails
            if (fails_of_packet++ != max_fails_of_packet)continue;

            this->is_receiving = false;
            return;
        }

    }

//        try {
//            unique_ptr<Packet> unq_pckt = file_transfer_socket->ReceivePacket();
//
//            // Reset fail count on valid packet, this isn't reached on timeout
//            fails_of_packet = 0;
//            cout << "<-- | RCV [" << unq_pckt->header->seqno << "]" << endl;
//            this->packets.push(unq_pckt.release());
//        }
//        catch (std::runtime_error &excption) {
//
//            if (!this->is_receiving) return;
//
//            cerr << "-*- | Timeout" << endl;
//
//            // TODO terminate of many fails
//            if (fails_of_packet++ != max_fails_of_packet)continue;
//
//            this->is_receiving = false;
//            return;
//        }
}


