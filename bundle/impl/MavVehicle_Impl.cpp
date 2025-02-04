#include "MavVehicle_Impl.h"
#include <iostream>
#include <mavsdk/base64.h>
#include <mavsdk/log_callback.h>
#include <mavsdk/plugins/shell/shell.h>

#pragma region public_method

MavVehicle_Impl::MavVehicle_Impl(MavVehicle::Config& config)
    : _config(config)
{
}

MavVehicle_Impl::~MavVehicle_Impl()
{
    Stop();
}

int MavVehicle_Impl::Exec()
{
#ifndef NDEBUG
    std::cout << "MAVLink URL    : " << _config.connect_url_ << std::endl;
    std::cout << " > External Log: " << _config.log_wo_sd_ << std::endl;
    std::cout << " > RAM Mission : " << _config.plan_wo_sd_ << std::endl;
#endif

    int result = 0;

    do {
        _mavsdk = std::make_shared<mavsdk::Mavsdk>(
            mavsdk::Mavsdk::Configuration(DEFAULT_MAV_SYS_ID, MAVMSG_COMP_ID_FCS, false));

        _mavsdk->intercept_incoming_messages_async(
            std::bind(&MavVehicle_Impl::onMavInterceptIncoming, this, std::placeholders::_1));

        if (_mavsdk->add_any_connection(_config.connect_url_) != mavsdk::ConnectionResult::Success)
        {
#ifndef NDEBUG
            std::cerr << "MAVLink Connection failed" << std::endl;
#endif
            result = -ECONNREFUSED;
            break;
        }

        _hmav_new_system = _mavsdk->subscribe_on_new_system(std::bind(&MavVehicle_Impl::onMavOnNewSystem, this));
    } while (false);

    return result;
}

void MavVehicle_Impl::Stop()
{
    _sigConnectionChanged.disconnect_all_slots();

    if (_system)
    {
        if (_system->is_connected())
            unsubscribe();
        _system->unsubscribe_is_connected(_hmav_is_connected);
    }

    _mavsdk->unsubscribe_on_new_system(_hmav_new_system);
    _mavsdk.reset();
}

int MavVehicle_Impl::PublishMessage(uint8_t target_cid, uint16_t msg_type, MAV::Message& msg_content)
{
    int result = 0;

    do {
        if (not _passthru)
        {
            result = -EUNATCH;
            break;
        }

        auto r = _passthru->queue_message([&](MavlinkAddress address, uint8_t channel)
                                          {
            mavlink_message_t message;
            mavlink_v2_extension_t extension {
                .message_type = msg_type,
                .target_network = MAVMSG_NET_ID_BROADCAST,
                .target_system = MAVMSG_SYS_ID_BROADCAST,
                .target_component = target_cid,
                .payload { 0 },
            };
            std::memcpy(extension.payload, msg_content.stream_, sizeof(msg_content.stream_));
            mavlink_msg_v2_extension_encode_chan(
                address.system_id,
                address.component_id,
                channel,
                &message,
                &extension
            );
            return message; });

        if (r != mavsdk::MavlinkPassthrough::Result::Success)
        {
            result = -EAGAIN;
            break;
        }
    } while (false);

    return result;
}

#pragma endregion public_method

#pragma region private_method

void MavVehicle_Impl::subscribe()
{
    _hmav_logging_raw = _logging->subscribe_log_streaming_raw(
        std::bind(&MavVehicle_Impl::onMavLogStreamingRaw, this, std::placeholders::_1));
    _hmav_extension_msg = _passthru->subscribe_message(
        MAVLINK_MSG_ID_V2_EXTENSION,
        std::bind(&MavVehicle_Impl::onMavMessageExtension, this, std::placeholders::_1));

    auto param = _param->get_param_int(PARAM_TOPIC_AIRFRAME);

    if (param.first == mavsdk::Param::Result::Success)
        _param_frame = param.second;

    if (_config.log_wo_sd_ or _config.plan_wo_sd_)
    {
        _thread = std::make_unique<std::thread>(
            [&]()
            {
                auto sh = std::make_shared<mavsdk::Shell>(_system);
                sh->subscribe_receive([](std::string) { });

                if (_config.log_wo_sd_)
                {
#ifndef NDEBUG
                    std::cout << "Extenal logging" << std::endl;
#endif
                    sh->send("logger stop\n");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    sh->send("logger start -m mavlink\n");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

                if (_config.plan_wo_sd_)
                {
#ifndef NDEBUG
                    std::cout << "Volatile mission" << std::endl;
#endif
                    sh->send("dataman stop\n");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    sh->send("dataman start -r\n");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            });
    }
}

void MavVehicle_Impl::unsubscribe()
{
    if (_system)
    {
        _logging->unsubscribe_log_streaming_raw(_hmav_logging_raw);
        _passthru->unsubscribe_message(MAVLINK_MSG_ID_V2_EXTENSION, _hmav_extension_msg);
    }
}

bool MavVehicle_Impl::onMavInterceptIncoming(mavlink_message_t& message)
{
    bool passed = true;

    do {
        if (message.msgid != MAVLINK_MSG_ID_V2_EXTENSION)
            break;

        mavlink_v2_extension_t extension;
        mavlink_msg_v2_extension_decode(&message, &extension);

        if (extension.target_component != MAVMSG_COMP_ID_FCS)
            break;

        std::basic_string<uint8_t> payload(extension.payload, std::size(extension.payload));
        _sigExtensionMessageArrived(extension.message_type, payload);

        passed = false;
    } while (false);

    return passed;
}

void MavVehicle_Impl::onMavOnNewSystem()
{
    do {
        // autopilot만 처리
        auto first_ap = _mavsdk->first_autopilot(1.);
        if (not first_ap)
            break;

        auto new_sys = first_ap.value();

        // 기존 연결되어 있다면, 새로운 연결 요청 cancel
        if (_system and _system->is_connected())
        {
            if (_system->get_system_id() == new_sys->get_system_id())
                break;
        }

    } while (false);
}

void MavVehicle_Impl::onMavSystemIsConnected(bool connected)
{
    if (connected)
    {
        _system->enable_timesync();

        _param    = std::make_shared<mavsdk::Param>(_system);
        _logging  = std::make_shared<mavsdk::LogStreaming>(_system);
        _passthru = std::make_shared<mavsdk::MavlinkPassthrough>(_system);

        subscribe();
    }
    else
    {
        if (_system)
            unsubscribe();
        _system->unsubscribe_is_connected(_hmav_is_connected);

        _param.reset();
        _logging.reset();
        _passthru.reset();
        _system.reset();
    }
}

void MavVehicle_Impl::onMavLogStreamingRaw(mavsdk::LogStreaming::LogStreamingRaw log_stream)
{
    if (_sigLogStreamingArrived.num_slots() > 0)
    {
        auto log = mavsdk::base64_decode(log_stream.data);
        if (log.size() > 0)
            _sigLogStreamingArrived(log);
    }
}

void MavVehicle_Impl::onMavMessageExtension(const mavlink_message_t& message)
{
    mavlink_v2_extension_t extension;
    mavlink_msg_v2_extension_decode(&message, &extension);

    if (extension.target_component == MAVMSG_COMP_ID_FCS)
    {
        std::basic_string<uint8_t> payload(extension.payload, std::size(extension.payload));
        _sigExtensionMessageArrived(extension.message_type, payload);
    }
}

#pragma endregion private_method
