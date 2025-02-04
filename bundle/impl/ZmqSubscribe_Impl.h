#pragma once

#include "ZmqSubscribe.h"
#include <atomic>
#include <boost/thread.hpp>
#include <zmq.hpp>

/**
 * @brief   ZmqSubscribe class의 implementation
 */
class ZmqSubscribe_Impl
{
#pragma region method_member

public:

    explicit ZmqSubscribe_Impl(ZmqSubscribe::Config& config);
    ~ZmqSubscribe_Impl();

    int  Exec();
    void Stop();

private:

    static constexpr auto RCV_TIMEOUT_MS = 200;

    void run(uint8_t vehicle_id);

    ZmqSubscribe::Config           _config;
    std::unique_ptr<boost::thread> _thread;
    zmq::context_t                 _zmq_context;
    zmq::socket_t                  _zmq_socket;

#pragma endregion method_member

#pragma region signal_slot

public:

    ZmqSubscribe::SigMessageArrived& SigMessageArrivedHandle() { return _sigMessageArrived; }

private:

    ZmqSubscribe::SigMessageArrived _sigMessageArrived;

#pragma endregion signal_slot
};
