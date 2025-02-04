/**
 * @subpage     MQTT Client
 * @author      imindude@gmail.com
 * @brief       MQTT 프로토콜을 이용한 정보 교환\n
 *              Paho-Mqtt-Cpp 사용
 */

#pragma once

#include "MqttMessage.h"
#include <boost/signals2.hpp>

class MqttClient_Impl;

/**
 * @brief   MQTT client (paho-mqtt-cpp 사용)
 */
class MqttClient
{
#pragma region method_member

public:

    /**
     * @brief   quality of service
     */
    enum class QoS
    {
        DONTCARE = 0, ///< 메세지를 한번만 전송 (수신여부는 신경쓰지 않음)
        MAYBE    = 1, ///< 최소 한번은 전송 및 수신 (여러번 중복 전송/수신 될 수 있음)
        SHOULDBE = 2, ///< 한번의 정확한 전송 및 수신을 보장 (기본값)
    };

    struct Config
    {
        std::string client_name_; ///< client name. 시스템 내에서 유일한 이름이어야 함.
        std::string broker_url_;  ///< 메세지 브로커 URL. ex) "tcp://127.0.0.1:1883"
    };

    explicit MqttClient(Config& config);
    ~MqttClient() = default;

    int  Exec();
    void Stop();

private:

    std::shared_ptr<MqttClient_Impl> _impl; ///< MqttClient 클래스의 implementation

#pragma endregion method_member

#pragma region signal_slot

public:

    /**
     * @brief   MQTT subscribe 메세지 수신 이벤트
     * @ingroup signals
     * @param   _1  topic ("MqttMessage.h")
     * @param   _2  message 바이트열 ("MavMessage.h")
     */
    using SigMessageArrived = boost::signals2::signal<void(std::string&, std::basic_string<uint8_t>&)>;

    /**
     * @ref     SigMessageArrived
     * @ingroup signals
     * @return  "SigMessageArrived" 핸들 리턴
     */
    SigMessageArrived& SigMessageArrivedHandle();

private:

#pragma endregion signal_slot
};
