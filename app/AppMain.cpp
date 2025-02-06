#include "AppMain.h"
#include "JsonKeys.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <thread>

#pragma region public_method

AppMain::AppMain(std::string& config_file)
{
    try
    {
        std::ifstream config_stream(config_file);
        Json::Value   config_value;

        if (config_stream.is_open())
        {
            Json::Reader json_reader;
            json_reader.parse(config_stream, config_value);
            config_stream.close();
        }

        // bundle

        MavVehicle::Config mav_config {
            .connect_url_ = config_value[JSON_KEY_MAVLINK_URL].asString(),
            .log_wo_sd_   = config_value[JSON_KEY_MAV_LOG_WO_SD].asBool(),
            .plan_wo_sd_  = config_value[JSON_KEY_MAV_PLAN_WO_SD].asBool(),
        };
        _mav_vehicle = std::make_shared<MavVehicle>(mav_config);

        // plugin

        Json::Value plugins = config_value[JSON_KEY_PLUGIN_LIST];

        for (auto plugin = plugins.begin(); plugin != plugins.end(); plugin++)
        {
            auto plugin_name = plugin->asString();

            if (plugin_name.compare(JSON_KEY_PLUGIN_SMALL_SWARM) == 0)
            {
                Json::Value json_value = config_value[JSON_KEY_SMALL_SWARM_SECTION];
                _pluginSmallSwarm(json_value);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int AppMain::Exec()
{
    MAV::Message feature_message {
        .feature_ { 0 }
    };

    startup(feature_message.feature_);

    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_DELAY_MS));

        _mav_vehicle->PublishMessage(MAVMSG_COMP_ID_GCS, MAVMSG_V2EXT_TYPE_FEATURE, feature_message);
    }

    return 0;
}

#pragma endregion public_method

#pragma region private_method

void AppMain::_pluginSmallSwarm(Json::Value& json)
{
    ZmqPublish::Config                pub_config;
    ZmqSubscribe::Config              sub_config;
    std::optional<ZmqPublish::Config> publish_option;
    std::vector<ZmqSubscribe::Config> subscribe_configs;

    if (json[JSON_KEY_SMALL_SWARM_LEADER].asBool())
    {
        pub_config.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
        pub_config.local_url_  = json[JSON_KEY_SMALL_SWARM_LEADER_URL].asString();
        publish_option         = pub_config;

        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_SUB_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_SUB_LEADER_URL].asString();
        subscribe_configs.push_back(sub_config);
    }
    else if (json[JSON_KEY_SMALL_SWARM_SUB_LEADER].asBool())
    {
        pub_config.vehicle_id_ = ZMQ_VEHICLE_ID_SUB_LEADER;
        pub_config.local_url_  = json[JSON_KEY_SMALL_SWARM_SUB_LEADER_URL].asString();
        publish_option         = pub_config;

        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_LEADER_URL].asString();
        subscribe_configs.push_back(sub_config);
    }
    else
    {
        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_LEADER_URL].asString();
        subscribe_configs.push_back(sub_config);

        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_SUB_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_SUB_LEADER_URL].asString();
        subscribe_configs.push_back(sub_config);
    }

    int vehicle_id    = json[JSON_KEY_SMALL_SWARM_MEMBER_NO].asInt();

    for (int n = 1; ; n++)
    {
        std::string connect_url = std::string(JSON_KEY_SMALL_SWARM_MEMBER_PARTIAL) + std::to_string(n + 1);

        if (not json[connect_url].empty())
        {
            if ((not publish_option.has_value()) and (vehicle_id == (n + 1)))
            {
                pub_config.vehicle_id_ = ZMQ_VEHICLE_ID_MEMBER_NUL + n;
                pub_config.local_url_  = json[connect_url].asString();
                publish_option         = pub_config;
            }
            else
            {
                sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_MEMBER_NUL + n;
                sub_config.remote_url_ = json[connect_url].asString();
                subscribe_configs.push_back(sub_config);
            }
        }
        else
        {
            break;
        }
    }

    if (not publish_option.has_value())
    {
#ifndef NDEBUG
        std::cerr << "0MQ Publisher is missing" << std::endl;
#endif
        throw -EBADR;
    }

    auto port = boost::lexical_cast<uint16_t>(pub_config.local_url_.substr(pub_config.local_url_.rfind(":") + 1));

    // publish의 경우, 로컬 주소를 0.0.0.0으로 변경해 줘야 함
    pub_config.local_url_ = std::string("tcp://0.0.0.0:") + std::to_string(port);

    _zmq_publish    = std::make_shared<ZmqPublish>(pub_config);
    _zmq_subscribes = std::make_shared<std::vector<ZmqSubscribe>>();

    for (auto it : subscribe_configs)
    {
        ZmqSubscribe subs(it);
        _zmq_subscribes->push_back(subs);
    }

    SmallSwarm::Config swarm_config {
        .vehicle_id_     = pub_config.vehicle_id_,
        .mav_vehicle_    = _mav_vehicle,
        .zmq_publish_    = _zmq_publish,
        .zmq_subscribes_ = _zmq_subscribes,
    };
    _small_swarm = std::make_unique<SmallSwarm>(swarm_config);
}

int AppMain::startup(MAV::Feature& feature)
{
    // bundles

    if (_mav_vehicle) // exception?
        _mav_vehicle->Exec();
    if (_mqtt_client) // exception?
        _mqtt_client->Exec();
    if (_zmq_publish) // exception?
        _zmq_publish->Exec();
    if (_zmq_subscribes) // exception?
    {
        for (auto it : *_zmq_subscribes)
            it.Exec();
    }

    // plugins

    if (_small_swarm)
    {
        _small_swarm->Exec();
        feature.small_swarm_ = true;
    }

    return 0;
}

#pragma endregion private_method
