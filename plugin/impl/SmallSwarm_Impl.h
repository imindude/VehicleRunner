#pragma once

#include "MavMessage.h"
#include "SmallSwarm.h"
#include "ZmqMessage.h"
#include <atomic>
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

    static constexpr auto LOOP_DELAY_MS = 100;

    void run();

    SmallSwarm::Config             _config;
    std::unique_ptr<boost::thread> _run_thread;

#pragma endregion method_member

#pragma region signal_slot

public:
private:

    void
    slotZmqMessageArrived(ZMQ::Message& msg_text);

#pragma endregion signal_slot
};
