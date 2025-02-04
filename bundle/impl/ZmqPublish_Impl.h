#pragma once

#include "ZmqPublish.h"
#include <zmq.hpp>

/**
 * @brief   ZmqPublish class의 implementation
 */
class ZmqPublish_Impl
{
#pragma region method_member

public:

    explicit ZmqPublish_Impl(ZmqPublish::Config& config);
    ~ZmqPublish_Impl();

    int  Exec();
    void Stop();
    int  Publish(ZMQ::Message& msg_text);

private:

    ZmqPublish::Config _config;
    zmq::context_t     _zmq_context;
    zmq::socket_t      _zmq_socket;
    std::mutex         _publish_lock;

#pragma endregion method_member

#pragma region signal_slot

public:
private:

#pragma endregion signal_slot
};
