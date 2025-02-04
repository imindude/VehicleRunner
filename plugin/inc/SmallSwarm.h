/**
 * @subpage     Small Swarm
 * @author      imindude@gmail.com
 * @brief       소규모 군집제어\n
 *              최대 5개의 vehicle에 대하여 편대 유지\n
 *              Leader(1) / Sub-Leader(1) / Member(2~3) 으로 구성되어 사전 정의한 편대의 형태를 유지
 */

#pragma once

#include "MavVehicle.h"
#include "ZmqPublish.h"
#include "ZmqSubscribe.h"

class SmallSwarm_Impl;

/**
 * @brief   소규모 군집제어를 위한 클래스
 */
class SmallSwarm
{
#pragma region method_member

public:

    struct Config
    {
        uint8_t                       vehicle_id_;    ///< 편대 구성원으로서의 지위 식별자. "ZmqMessage.h" 참고.
        std::shared_ptr<MavVehicle>   mav_vehicle_;   ///< MavVehicle의 instance.
        std::shared_ptr<ZmqPublish>   zmq_publish_;   ///< 편대 구성원에게 메세지 전달
        std::shared_ptr<ZmqSubscribe> zmq_subscribe_; ///< 편대 구성원(들)의 메세지 수신
    };

    explicit SmallSwarm(Config& config);
    ~SmallSwarm() = default;

    int  Exec();
    void Stop();

private:

    std::unique_ptr<SmallSwarm_Impl> _impl; ///< SmallSwarm 클래스의 implementation

#pragma endregion method_member

#pragma region signal_slot

public:
private:

#pragma endregion signal_slot
};

// EOF
