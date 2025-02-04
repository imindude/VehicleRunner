#include "MqttClient_Impl.h"

#pragma region public_method

MqttClient::MqttClient(Config& config)
{
    _impl = std::make_shared<MqttClient_Impl>(config);
}

int MqttClient::Exec()
{
    return _impl->Exec();
}

void MqttClient::Stop()
{
    return _impl->Stop();
}

#pragma endregion public_method

#pragma region public_signal

MqttClient::SigMessageArrived& MqttClient::SigMessageArrivedHandle()
{
    return _impl->SigMessageArrivedHandle();
}

#pragma endregion public_signal
