#include "ZmqSubscribe_Impl.h"

#pragma region public_method

ZmqSubscribe::ZmqSubscribe(Config& config)
{
    _impl = std::make_shared<ZmqSubscribe_Impl>(config);
}

int ZmqSubscribe::Exec()
{
    return _impl->Exec();
}

void ZmqSubscribe::Stop()
{
    return _impl->Stop();
}

#pragma endregion public_method

#pragma region public_signal

ZmqSubscribe::SigMessageArrived& ZmqSubscribe::SigMessageArrivedHandle()
{
    return _impl->SigMessageArrivedHandle();
}

#pragma endregion public_signal
