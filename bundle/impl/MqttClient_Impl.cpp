#include "MqttClient_Impl.h"
#include <iostream>

#pragma region public_method

MqttClient_Impl::MqttClient_Impl(MqttClient::Config& config)
    : _config(config)
{
}

MqttClient_Impl::~MqttClient_Impl()
{
    Stop();
}

int MqttClient_Impl::Exec()
{
#ifndef NDEBUG
    std::cout << "MQTT Client Name: " << _config.client_name_ << std::endl;
    std::cout << "MQTT Broker URL : " << _config.broker_url_ << std::endl;
#endif

    auto mqtt_options = mqtt::connect_options_builder().clean_session().automatic_reconnect().finalize();

    _mqtt_client   = std::make_unique<mqtt::async_client>(_config.broker_url_, _config.client_name_);
    _mqtt_callback = std::make_unique<mqttCallback>(this);

    _mqtt_client->set_callback(*_mqtt_callback);

    return (_mqtt_client->connect(mqtt_options) == nullptr) ? -ECONNREFUSED : 0;
}

void MqttClient_Impl::Stop()
{
    _sigMessageArrived.disconnect_all_slots();

    if (_mqtt_client)
    {
        for (auto it : _subscribed_topic_list)
            _mqtt_client->unsubscribe(it.first);
        _subscribed_topic_list.clear();
        _lazy_subscription_list.clear();
        _mqtt_client->disable_callbacks();
        _mqtt_client->disconnect();
    }

    _mqtt_client.reset();
    _mqtt_callback.reset();
}

#pragma endregion public_method

#pragma region private_method

void MqttClient_Impl::publish(const char* topic, void* data, size_t size, int qos, bool retained)
{
    if (_mqtt_client and _mqtt_client->is_connected())
    {
        std::lock_guard<std::mutex> locker(_publish_lock);
        _mqtt_client->publish(topic, data, size, qos, retained);
    }
}

void MqttClient_Impl::subscribe(const char* topic, MqttClient::QoS qos)
{
    bool registered(false);

    if (_mqtt_client and _mqtt_client->is_connected())
    {
        for (auto it : _subscribed_topic_list)
        {
            if (it.first == topic)
            {
                registered = true;
                break;
            }
        }

        if (not registered)
        {
            std::pair<std::string, int> node(topic, static_cast<int>(qos));
            _subscribed_topic_list.push_back(node);
            _mqtt_client->subscribe(node.first, node.second);
        }
    }
    else
    {
        for (auto it : _lazy_subscription_list)
        {
            if (it.first == topic)
            {
                registered = true;
                break;
            }
        }

        if (not registered)
        {
            std::pair<std::string, int> node(topic, static_cast<int>(qos));
            _lazy_subscription_list.push_back(node);
        }
    }
}

void MqttClient_Impl::subscribeLazy()
{
    for (auto it : _lazy_subscription_list)
    {
        std::pair<std::string, int> node(it.first, it.second);
        _subscribed_topic_list.push_back(node);
        _mqtt_client->subscribe(node.first, node.second);
    }
    _subscribed_topic_list.clear();
}

void MqttClient_Impl::connectionLost()
{
    for (auto it : _subscribed_topic_list)
        _lazy_subscription_list.push_back(it);
    _subscribed_topic_list.clear();
}

#pragma endregion private_method

#pragma region inner_class

void MqttClient_Impl::mqttCallback::connected([[maybe_unused]] const std::string& cause)
{
#ifndef NDEBUG
    std::cout << "MQTT connected: " << std::endl;
#endif
    _parent->subscribeLazy();
}

void MqttClient_Impl::mqttCallback::connection_lost([[maybe_unused]] const std::string& cause)
{
#ifndef NDEBUG
    std::cout << "MQTT connection lost: " << std::endl;
#endif
    _parent->connectionLost();
}

void MqttClient_Impl::mqttCallback::message_arrived(mqtt::const_message_ptr message)
{
    std::string                topic(message->get_topic());
    std::basic_string<uint8_t> context(message->get_payload().size(), 0);

    std::memcpy(context.data(), message->get_payload().data(), context.size());
    _parent->_sigMessageArrived(topic, context);
}

void MqttClient_Impl::mqttCallback::delivery_complete([[maybe_unused]] mqtt::delivery_token_ptr token) { }

void MqttClient_Impl::mqttListener::on_success([[maybe_unused]] const mqtt::token& token) { }

void MqttClient_Impl::mqttListener::on_failure([[maybe_unused]] const mqtt::token& token) { }

#pragma endregion inner_class
