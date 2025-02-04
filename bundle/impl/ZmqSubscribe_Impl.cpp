#include "ZmqSubscribe_Impl.h"
#include "Checksum.h"
#include <iostream>

#pragma region public_method

ZmqSubscribe_Impl::ZmqSubscribe_Impl(ZmqSubscribe::Config& config)
    : _config(config)
    , _zmq_context(1)
    , _zmq_socket(_zmq_context, zmq::socket_type::sub)
{
}

ZmqSubscribe_Impl::~ZmqSubscribe_Impl()
{
    Stop();
}

int ZmqSubscribe_Impl::Exec()
{
#ifndef NDEBUG
    std::cout << "0MQ Subscribe URL : " << _config.remote_url_ << std::endl;
#endif

    int const timeout_ms = RCV_TIMEOUT_MS;

    _zmq_socket.set(zmq::sockopt::rcvtimeo, timeout_ms);
    _zmq_socket.set(zmq::sockopt::subscribe, ""); // pub/sub 모델에서 subscribe는 반드시 "topic"을 등록해야 함
    _zmq_socket.connect(_config.remote_url_);     // 연결은 0MQ 라이브러리가 알아서 해결함

    _thread = std::make_unique<boost::thread>(boost::bind(&ZmqSubscribe_Impl::run, this, _config.vehicle_id_));

    return 0;
}

void ZmqSubscribe_Impl::Stop()
{
    if (_thread)
    {
        _thread->interrupt();
        _thread->join();
        _thread.reset();

        _zmq_socket.disconnect(_config.remote_url_);
        _zmq_socket.close();
        _zmq_context.shutdown();
        _zmq_context.close();
    }
}

#pragma endregion public_method

#pragma region private_method

void ZmqSubscribe_Impl::run(uint8_t vehicle_id)
{
    zmq::message_t message;
    ZMQ::Packet    packet;

    while (not _thread->interruption_requested())
    {
        auto result = _zmq_socket.recv(message, zmq::recv_flags::none);

        if (result.has_value() and (result.value() > 0))
        {
            auto size = result.value();

            std::memcpy(&packet, message.data(), size);
            if (packet.header_.vehicle_id_ != vehicle_id)
                continue;

            ZMQ::Footer footer {
                .checksum_ = Checksum::crc8ccitt(reinterpret_cast<uint8_t*>(&packet), size - sizeof(ZMQ::Footer)),
            };
            if (footer.checksum_ == packet.footer_.checksum_)
                _sigMessageArrived(packet);

            message.rebuild();
        }
    }
}

#pragma endregion private_method
