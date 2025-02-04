/**
 * @subpage     ZeroMQ Subscriber
 * @author      imindude@gmail.com
 * @brief       ZeroMQ를 이용하여 메세지 subscribing (pub/sub모델)\n
 *              ZMQ 라이브러리 사용
 */

#pragma once

#include "ZmqMessage.h"
#include <boost/signals2.hpp>

class ZmqSubscribe_Impl;

/**
 * @brief   0MQ를 이용하여 타 vehicle의 정보를 수신
 */
class ZmqSubscribe
{
#pragma region method_member

public:

    struct Config
    {
        uint8_t     vehicle_id_; ///< vehicle 식별자. "ZmqMessage.h"
        std::string remote_url_; ///< subscribe 노드의 정보 제공측 URL
    };

    explicit ZmqSubscribe(Config& config);
    ~ZmqSubscribe() = default;

    int  Exec();
    void Stop();

private:

    std::shared_ptr<ZmqSubscribe_Impl> _impl; ///< ZmqSubscribe 클래스의 implementation

#pragma endregion method_member

#pragma region signal_slot

public:

    /**
     * @brief   0MQ의 subscribe에서 수신한 메세지
     * @ingroup signals
     * @param   _1  수신 패킷
     */
    using SigMessageArrived = boost::signals2::signal<void(ZMQ::Message&)>;

    /**
     * @ref     SigMessageArrived
     * @ingroup signals
     * @return  "SigMessageArrived" 핸들 리턴
     */
    SigMessageArrived& SigMessageArrivedHandle();

private:

#pragma endregion signal_slot
};
