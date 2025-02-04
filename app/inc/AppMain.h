/**
 * @mainpage    Vehicle Runner
 * @author      imindude@gmail.com
 * @brief       MAVLink를 이용한 AutoPilot 구현\n
 *              0MQ 및 MQTT등을 이용하여 각 Vehicle간의 통신
 *
 * @defgroup    signals     boost의 signal/slot을 이용한 이벤트 처리
 */

#pragma once

#include "MavVehicle.h"
#include "MqttClient.h"
#include "SmallSwarm.h"
#include "ZmqPublish.h"
#include "ZmqSubscribe.h"
#include <jsoncpp/json/json.h>

/**
 * @brief   Main Entry
 */
class AppMain
{
#pragma region method_member

public:

    explicit AppMain(std::string& config_file);
    ~AppMain() = default;

    int Exec();

private:

    static constexpr auto LOOP_DELAY_MS = 2000;

    /**
     * @brief   소규모 군집제어 플러그인 startup\n
     *          생성자에서 호출되는 메소드
     * @param   json    SmallSwarm startup을 위한 json타입 config.
     * @return  0:success, other:failed
     */
    void _pluginSmallSwarm(Json::Value& json);
    /**
     * @brief   plugin 및 bundle 시작
     * @param   feature     GCS로 전달하기 위한 활성화된 plugin 목록 리스트
     * @return  0:success, other:failed
     */
    int startup(MAV::Feature& feature);

    // bundles

    std::shared_ptr<MavVehicle>                _mav_vehicle { nullptr };    ///< MavVehicle 인스턴스
    std::shared_ptr<MqttClient>                _mqtt_client { nullptr };    ///< MqttClient 인스턴스
    std::shared_ptr<ZmqPublish>                _zmq_publish { nullptr };    ///< 내 vehicle 정보 제공용 0MQ 인스턴스
    std::shared_ptr<std::vector<ZmqSubscribe>> _zmq_subscribes { nullptr }; ///< 타 vehicle 정보 수신용 0MQ 인스턴스들

    // plugins

    std::unique_ptr<SmallSwarm> _small_swarm { nullptr }; ///< SmallSwarm 인스턴스 (소규모 군집제어)

#pragma endregion method_member
};
