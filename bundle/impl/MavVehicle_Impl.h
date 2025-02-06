#pragma once

#include "MavVehicle.h"
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/log_streaming/log_streaming.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/system.h>
#include <thread>

/**
 * @brief   MavVehicle class의 implementation
 */
class MavVehicle_Impl
{
#pragma region method_member

public:

    explicit MavVehicle_Impl(MavVehicle::Config& config);
    ~MavVehicle_Impl();

    int  Exec();
    void Stop();
    int  PublishMessage(uint8_t target_cid, uint16_t msg_type, MAV::Message& msg_text);
    int  GetPosition(MavVehicle::PositionLlh& llh);
    int  GetVelocity(MavVehicle::VelocityNed& ned);

private:

    static constexpr auto DEFAULT_MAV_SYS_ID = 250;

    static constexpr auto PARAM_TOPIC_AIRFRAME = "CA_AIRFRAME"; // int

    void subscribe();
    void unsubscribe();

    bool onMavInterceptIncoming(mavlink_message_t& message);
    void onMavOnNewSystem();
    void onMavSystemIsConnected(bool connected);
    void onMavLogStreamingRaw(mavsdk::LogStreaming::LogStreamingRaw log_stream);
    void onMavMessageExtension(const mavlink_message_t& message);

    MavVehicle::Config                          _config;
    std::unique_ptr<std::thread>                _thread { nullptr };
    int                                         _param_frame { 0 };
    std::shared_ptr<mavsdk::Mavsdk>             _mavsdk;
    std::shared_ptr<mavsdk::System>             _system;
    std::shared_ptr<mavsdk::Param>              _param;
    std::shared_ptr<mavsdk::Telemetry>          _telemetry;
    std::shared_ptr<mavsdk::LogStreaming>       _logging;
    std::shared_ptr<mavsdk::MavlinkPassthrough> _passthru;
    mavsdk::Mavsdk::NewSystemHandle             _hmav_new_system;
    mavsdk::System::IsConnectedHandle           _hmav_is_connected;
    mavsdk::LogStreaming::LogStreamingRawHandle _hmav_logging_raw;
    mavsdk::MavlinkPassthrough::MessageHandle   _hmav_extension_msg;

#pragma endregion method_member

#pragma region signal_slot

public:

    MavVehicle::SigConnectionChanged&       SigConnectionChangedHandle() { return _sigConnectionChanged; }
    MavVehicle::SigLogStreamingArrived&     SigLogStreamingArrivedHandle() { return _sigLogStreamingArrived; }
    MavVehicle::SigExtensionMessageArrived& SigExtensionMessageArrivedHandle() { return _sigExtensionMessageArrived; }

private:

    MavVehicle::SigConnectionChanged       _sigConnectionChanged;
    MavVehicle::SigLogStreamingArrived     _sigLogStreamingArrived;
    MavVehicle::SigExtensionMessageArrived _sigExtensionMessageArrived;

#pragma endregion signal_slot
};
