#pragma once

#include "MavMessage.h"
#include "SmallSwarm.h"
#include "ZmqMessage.h"

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

    SmallSwarm::Config _config;

#pragma endregion method_member

#pragma region signal_slot

public:
private:

#pragma endregion signal_slot
};
