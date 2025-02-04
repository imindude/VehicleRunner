#include "AppMain.h"
#include "JsonKeys.h"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <thread>

#pragma region public_method

AppMain::AppMain(std::string& config_file)
    : _config_file(config_file)
{
}

int AppMain::Exec()
{
    try
    {
        std::ifstream config_stream(_config_file);
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
                pluginSmallSwarm(json_value);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    // ZMQ::Packet packet;
    // std::memset(reinterpret_cast<void*>(&packet), 0, sizeof(packet));

    // packet.header_.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
    // packet.header_.message_id_ = ZMQ_MSG_VEHICLE_POSITION;
    // packet.payload_.stream_[0] = 3;
    // packet.payload_.stream_[1] = 4;
    // packet.payload_.stream_[2] = 5;
    // packet.payload_.stream_[3] = 6;

    for (;;)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_DELAY_MS));

        /// @todo   mav_vehicle로 mavlink v2 extension 메세지를 전송하도록 (어디로?)
    }

    return 0;
}

#pragma endregion public_method

#pragma region private_method

int AppMain::pluginSmallSwarm(Json::Value& json)
{
    ZmqPublish::Config                pub_config;
    ZmqSubscribe::Config              sub_config;
    std::optional<ZmqPublish::Config> publish_option;
    std::vector<ZmqSubscribe::Config> sub_config_list;

    if (json[JSON_KEY_SMALL_SWARM_LEADER].asBool() == true)
    {
        pub_config.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
        pub_config.local_url_  = json[JSON_KEY_SMALL_SWARM_LEADER_URL].asString();
        publish_option         = pub_config;

        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_SUB_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_SUB_LEADER_URL].asString();
        sub_config_list.push_back(sub_config);
    }
    else if (json[JSON_KEY_SMALL_SWARM_SUB_LEADER].asBool() == true)
    {
        pub_config.vehicle_id_ = ZMQ_VEHICLE_ID_SUB_LEADER;
        pub_config.local_url_  = json[JSON_KEY_SMALL_SWARM_SUB_LEADER_URL].asString();
        publish_option         = pub_config;

        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_LEADER_URL].asString();
        sub_config_list.push_back(sub_config);
    }
    else
    {
        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_LEADER_URL].asString();
        sub_config_list.push_back(sub_config);

        sub_config.vehicle_id_ = ZMQ_VEHICLE_ID_SUB_LEADER;
        sub_config.remote_url_ = json[JSON_KEY_SMALL_SWARM_SUB_LEADER_URL].asString();
        sub_config_list.push_back(sub_config);
    }

    int vehicle_id    = json[JSON_KEY_SMALL_SWARM_MEMBER_NO].asInt();
    int member_list[] = { ZMQ_VEHICLE_ID_MEMBER_1, ZMQ_VEHICLE_ID_MEMBER_2, ZMQ_VEHICLE_ID_MEMBER_3 };

    for (int n = 0; n < 3; n++)
    {
        std::string connect_url = std::string(JSON_KEY_SMALL_SWARM_MEMBER_PARTIAL) + std::to_string(n + 1);

        if ((not publish_option.has_value()) and (vehicle_id == (n + 1)))
        {
            pub_config.vehicle_id_ = member_list[n];
            pub_config.local_url_  = json[connect_url].asString();
            publish_option         = pub_config;
        }
        else
        {
            sub_config.vehicle_id_ = member_list[n];
            sub_config.remote_url_ = json[connect_url].asString();
            sub_config_list.push_back(sub_config);
        }
    }

    if (not publish_option.has_value())
    {
        std::cerr << "0MQ Publisher is missing" << std::endl;
        return -1;
    }

    auto port = boost::lexical_cast<uint16_t>(pub_config.local_url_.substr(pub_config.local_url_.rfind(":") + 1));

    pub_config.local_url_ = std::string("tcp://0.0.0.0:") + std::to_string(port);

    _zmq_publish = std::make_shared<ZmqPublish>(pub_config);
    _zmq_publish->Exec();

    for (auto it : sub_config_list)
    {
        ZmqSubscribe subs(it);
        _zmq_subs_list.push_back(subs);
        subs.Exec();
    }

    return 0;
}

#pragma endregion private_method
