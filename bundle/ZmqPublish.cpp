#include "ZmqPublish_Impl.h"

#pragma region public_method

ZmqPublish::ZmqPublish(Config& config)
{
    _impl = std::make_shared<ZmqPublish_Impl>(config);
}

int ZmqPublish::Exec()
{
    return _impl->Exec();
}

void ZmqPublish::Stop()
{
    return _impl->Stop();
}

int ZmqPublish::Publish(ZMQ::Packet& packet)
{
    return _impl->Publish(packet);
}

#pragma endregion public_method

#pragma region public_signal
#pragma endregion public_signal
