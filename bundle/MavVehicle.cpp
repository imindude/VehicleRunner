#include "MavVehicle_Impl.h"

#pragma region public_method

MavVehicle::MavVehicle(Config& config)
{
    _impl = std::make_shared<MavVehicle_Impl>(config);
}

int MavVehicle::Exec()
{
    return _impl->Exec();
}

void MavVehicle::Stop()
{
    return _impl->Stop();
}

int MavVehicle::PublishMessage(uint8_t target_cid, uint16_t msg_type, MAV::Message& msg_content)
{
    return _impl->PublishMessage(target_cid, msg_type, msg_content);
}

#pragma endregion public_method

#pragma region public_signal

MavVehicle::SigConnectionChanged& MavVehicle::SigConnectionChangedHandle()
{
    return _impl->SigConnectionChangedHandle();
}

MavVehicle::SigLogStreamingArrived& MavVehicle::SigLogStreamingArrivedHandle()
{
    return _impl->SigLogStreamingArrivedHandle();
}

MavVehicle::SigExtensionMessageArrived& MavVehicle::SigExtensionMessageArrivedHandle()
{
    return _impl->SigExtensionMessageArrivedHandle();
}

#pragma endregion public_signal
