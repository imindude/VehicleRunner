#pragma once

#include "MqttClient.h"
#include <mqtt/async_client.h>

/**
 * @brief   MqttClient class의 implementation
 */
class MqttClient_Impl
{
    class mqttCallback;

#pragma region method_member

public:

    explicit MqttClient_Impl(MqttClient::Config& config);
    ~MqttClient_Impl();

    int  Exec();
    void Stop();

private:

    void publish(const char* topic, void* data, size_t size, int qos, bool retained);
    void subscribe(const char* topic, MqttClient::QoS qos);
    void subscribeLazy();
    void connectionLost();

    MqttClient::Config                     _config;
    std::unique_ptr<mqtt::async_client>    _mqtt_client;
    std::unique_ptr<mqttCallback>          _mqtt_callback;
    std::mutex                             _publish_lock;
    std::list<std::pair<std::string, int>> _subscribed_topic_list;
    std::list<std::pair<std::string, int>> _lazy_subscription_list;

#pragma endregion method_member

#pragma region signal_slot

public:

    MqttClient::SigMessageArrived& SigMessageArrivedHandle() { return _sigMessageArrived; }

private:

    MqttClient::SigMessageArrived _sigMessageArrived;

#pragma endregion signal_slot

#pragma region inner_class

private:

    /**
     * @brief   paho-mqtt-cpp 의 callback 클래스를 상속받아서 이벤트 처리
     */
    class mqttCallback : public virtual mqtt::callback
    {
    public:

        mqttCallback(MqttClient_Impl* parent)
            : _parent(parent)
        {
        }
        ~mqttCallback() = default;

        void connected(const std::string& cause) override;
        void connection_lost(const std::string& cause) override;
        void message_arrived(mqtt::const_message_ptr message) override;
        void delivery_complete(mqtt::delivery_token_ptr token) override;

    private:

        MqttClient_Impl* _parent { nullptr }; ///< outer클래스에 이벤트를 전달하기 위한 핸들
    };

    /**
     * @brief   paho-mqtt-cpp에서 메세지 송수신 결과 처리 (사용 X, 기록용)
     */
    class mqttListener : public virtual mqtt::iaction_listener
    {
    public:

        mqttListener(MqttClient_Impl* parent)
            : _parent(parent)
        {
        }
        ~mqttListener() = default;

        void on_success(const mqtt::token& token) override;
        void on_failure(const mqtt::token& token) override;

    private:

        MqttClient_Impl* _parent { nullptr }; ///< outer클래스에 이벤트를 전달하기 위한 핸들
    };

#pragma endregion inner_class
};
