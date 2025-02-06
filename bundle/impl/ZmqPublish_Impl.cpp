#include "ZmqPublish_Impl.h"
#include "Checksum.h"
#include <iostream>

#pragma region public_method

ZmqPublish_Impl::ZmqPublish_Impl(ZmqPublish::Config& config)
    : _config(config)
    , _zmq_context(1)
    , _zmq_socket(_zmq_context, zmq::socket_type::pub)
{
}

ZmqPublish_Impl::~ZmqPublish_Impl()
{
    Stop();
}

int ZmqPublish_Impl::Exec()
{
#ifndef NDEBUG
    std::cout << "0MQ[" << (int)_config.vehicle_id_ << "] Publish URL : " << _config.local_url_ << std::endl;
#endif
    _zmq_socket.bind(_config.local_url_);
    return 0;
}

void ZmqPublish_Impl::Stop()
{
    _zmq_socket.unbind(_config.local_url_);
    _zmq_socket.close();
    _zmq_context.shutdown();
    _zmq_context.close();
}

int ZmqPublish_Impl::Publish(uint8_t msg_id, ZMQ::Payload& payload)
{
    int result = 0;

    do {
        if (msg_id != ZMQ_MSG_VEHICLE_PAYLOAD)
        {
            result = -ENOMSG;
            break;
        }

        ZMQ::Message message {
            .header_ {
                .vehicle_id_ = _config.vehicle_id_,
                .message_id_ = msg_id,
            },
            .payload_ = payload,
            .footer_ { 0 },
        };
        message.footer_.checksum_ = Checksum::crc8ccitt(
            reinterpret_cast<uint8_t*>(&message), sizeof(ZMQ::Message) - sizeof(ZMQ::Footer));

        zmq::message_t packet(&message, sizeof(ZMQ::Message));
        {
            std::lock_guard<std::mutex> locker(_publish_lock);
            _zmq_socket.send(packet, zmq::send_flags::none);
        }
    } while (false);

    return result;
}

#pragma endregion public_method
