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
    std::cout << "0MQ Publish URL : " << _config.local_url_ << std::endl;
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

int ZmqPublish_Impl::Publish(ZMQ::Packet& packet)
{
    int result = 0;

    do {
        if (packet.header_.vehicle_id_ != _config.vehicle_id_)
        {
            result = -EPERM;
            break;
        }

        size_t message_size = 0;

        switch (packet.header_.message_id_)
        {
        case ZMQ_MSG_VEHICLE_POSITION:
            message_size = sizeof(ZMQ::Position);
            break;
        case ZMQ_MSG_VEHICLE_VELOCITY:
            message_size = sizeof(ZMQ::Velocity);
            break;
        default:
            break;
        }

        if (message_size == 0)
        {
            result = -ENXIO;
            break;
        }

        message_size += sizeof(ZMQ::Header);

        zmq::message_t message(message_size + sizeof(ZMQ::Footer));
        ZMQ::Footer    footer {
            .checksum_ = Checksum::crc8ccitt(reinterpret_cast<uint8_t*>(&packet), message_size),
        };

        std::memcpy(message.data(), &packet, message_size);
        std::memcpy(static_cast<uint8_t*>(message.data()) + message_size, &footer, sizeof(footer));

        {
            std::lock_guard<std::mutex> locker(_publish_lock);
            _zmq_socket.send(message, zmq::send_flags::none);
        }
    } while (false);

    return result;
}

#pragma endregion public_method
