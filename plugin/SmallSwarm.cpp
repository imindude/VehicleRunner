#include "SmallSwarm_Impl.h"
#include <iostream>

#pragma region public_method

SmallSwarm::SmallSwarm(Config& config)
{
    _impl = std::make_shared<SmallSwarm_Impl>(config);
}

int SmallSwarm::Exec()
{
    return _impl->Exec();
}

void SmallSwarm::Stop()
{
    return _impl->Stop();
}

#pragma endregion public_method
