#include "SmallSwarm_Impl.h"
#include <iostream>

#pragma region public_method

SmallSwarm_Impl::SmallSwarm_Impl(SmallSwarm::Config& config)
    : _config(config)
{
}

SmallSwarm_Impl::~SmallSwarm_Impl()
{
    Stop();
}

int SmallSwarm_Impl::Exec()
{
#ifndef NDEBUG
    std::cout << "SmallSwarm Vehicle ID : " << _config.vehicle_id_ << std::endl;
#endif

    return 0;
}

void SmallSwarm_Impl::Stop()
{
}

#pragma endregion public_method
