#pragma once

#include "MavMessage.h"
#include "SmallSwarm.h"
#include "ZmqMessage.h"
#include <atomic>
#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>

/**
 * @brief   SmallSwarm의 implementation
 */
class SmallSwarm_Impl
{
#pragma region method_member

public:

    explicit SmallSwarm_Impl(SmallSwarm::Config& config);
    ~SmallSwarm_Impl();

    int  Exec();
    void Stop();

private:

    struct EventText
    {
        ZMQ::Header  metadata_;
        ZMQ::Payload payload_;
    };

    static constexpr auto PUBLISH_TERM_MS = 200; ///< 초당 5번 메세지 publishing
    static constexpr auto EVENT_WAIT_MS   = 100; ///< 이벤트 발생시까지 기다리는 최대 시간
    static constexpr auto EVENT_RING_SIZE = 10;  ///< 이벤트 버퍼 크기 (4개의 노드에서 초당 5회 이벤트 발생)

    void run();
    void publish(MavVehicle::PositionLlh& llh, MavVehicle::VelocityNed& ned);

    SmallSwarm::Config                _config;
    std::unique_ptr<boost::thread>    _run_thread;
    boost::circular_buffer<EventText> _event_buffer { EVENT_RING_SIZE };
    std::mutex                        _event_buffer_lock;

#pragma endregion method_member

#pragma region signal_slot

public:
private:

    void slotZmqMessageArrived(ZMQ::Message& msg_text);

#pragma endregion signal_slot
};
