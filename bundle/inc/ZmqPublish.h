/**
 * @subpage     ZeroMQ Publisher
 * @author      imindude@gmail.com
 * @brief       ZeroMQ를 이용하여 메세지 publishing (pub/sub모델)\n
 *              ZMQ 라이브러리 사용
 */

#pragma once

#include "ZmqMessage.h"
#include <memory>

class ZmqPublish_Impl;

/**
 * @brief   0MQ를 이용하여 자신의 정보를 타 vehicle에 publishing
 */
class ZmqPublish
{
#pragma region method_member

public:

    struct Config
    {
        uint8_t     vehicle_id_; ///< vehicle 식별자. "ZmqMessage.h"
        std::string local_url_;  ///< publish 노드의 binding URL. ex) "tcp://127.0.0.1:5563"
    };

    explicit ZmqPublish(Config& config);
    ~ZmqPublish() = default;

    int  Exec();
    void Stop();
    /**
     * @brief   ZeroMQ 메세지 전송
     * @param   msg_id      메세지 식별자
     * @param   payload     메세지 내용
     * @return  0:success, other:failed
     */
    int  Publish(uint8_t msg_id, ZMQ::Payload& payload);

private:

    std::shared_ptr<ZmqPublish_Impl> _impl; ///< ZmqPublish 클래스의 implementation

#pragma endregion method_member

#pragma region signal_slot

public:
private:

#pragma endregion signal_slot
};
