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
    std::cout << "SmallSwarm Vehicle ID : " << (int)_config.vehicle_id_ << std::endl;
#endif

    _run_thread = std::make_unique<boost::thread>(boost::bind(&SmallSwarm_Impl::run, this));

    for (auto it : *_config.zmq_subscribes_)
    {
        it.SigMessageArrivedHandle().connect(
            boost::bind(&SmallSwarm_Impl::slotZmqMessageArrived, this, boost::placeholders::_1));
    }

    return 0;
}

void SmallSwarm_Impl::Stop()
{
}

#pragma endregion public_method

#pragma region private_method

void SmallSwarm_Impl::run()
{
    ZMQ::Message message_text;

    message_text.header_.vehicle_id_ = _config.vehicle_id_,
    message_text.header_.message_id_ = ZMQ_MSG_UNKNOWN_MESSAGE;

    // for stabilization delay
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

    while (not _run_thread->interruption_requested())
    {
        // boost::this_thread::sleep_for(boost::chrono::milliseconds(LOOP_DELAY_MS));
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

        int64_t now_ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(
                             boost::chrono::system_clock::now().time_since_epoch())
                             .count();

        message_text.header_.message_id_ = ZMQ_MSG_VEHICLE_POSITION;
        std::memcpy(&message_text.payload_, &now_ms, sizeof(now_ms));

        _config.zmq_publish_->Publish(message_text);
        std::cout << "0MQ_PUB[" << (int)message_text.header_.vehicle_id_ << "] " << now_ms << std::endl;
    }
}

#pragma endregion private_method

#pragma region signal_slot

void SmallSwarm_Impl::slotZmqMessageArrived(ZMQ::Message& msg_text)
{
    if (msg_text.header_.message_id_ == ZMQ_MSG_VEHICLE_POSITION)
    {
        int64_t millis;

        std::memcpy(&millis, &msg_text.payload_, sizeof(millis));
        std::cout << "0MQ_SUB[" << (int)msg_text.header_.vehicle_id_ << "] " << millis << std::endl;
    }
}

#pragma endregion signal_slot
