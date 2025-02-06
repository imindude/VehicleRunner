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
    if (_run_thread)
    {
        _run_thread->interrupt();
        _run_thread->join();
        _run_thread.reset();
    }
}

#pragma endregion public_method

#pragma region private_method

void SmallSwarm_Impl::run()
{
    MavVehicle::PositionLlh position_llh;
    MavVehicle::VelocityNed velocity_ned;
    EventText               event_text;

    int64_t now_ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(
                         boost::chrono::system_clock::now().time_since_epoch())
                         .count();
    int64_t pub_ms = now_ms;

    // for stabilization delay
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

    while (not _run_thread->interruption_requested())
    {
        if (not _event_buffer.empty())
        {
            {
                std::lock_guard<std::mutex> locker(_event_buffer_lock);
                event_text = _event_buffer.front();
                _event_buffer.pop_front();
            }

            /// @todo vehicle정보 읽고, 적당히 처리하는거 여기
        }
        else
        {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(EVENT_WAIT_MS));
        }

        now_ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(
                     boost::chrono::system_clock::now().time_since_epoch())
                     .count();

        if ((now_ms - pub_ms) >= PUBLISH_TERM_MS)
        {
            _config.mav_vehicle_->GetPosition(position_llh);
            _config.mav_vehicle_->GetVelocity(velocity_ned);

            publish(position_llh, velocity_ned);
            pub_ms = now_ms;
        }
    }
}

void SmallSwarm_Impl::publish(MavVehicle::PositionLlh& llh, MavVehicle::VelocityNed& ned)
{
    ZMQ::Payload payload {
        .position_ {
            .latitude_deg_  = llh.latitude_deg_,
            .longitude_deg_ = llh.longitude_deg_,
            .altitude_m_    = llh.rel_altitude_m_, /// llh.msl_altitude_m_ 을 어떻게 쓸지 고민 필요
        },
        .velocity_ {
            .north_mps_ = ned.north_mps_,
            .east_mps_  = ned.east_mps_,
            .down_mps_  = ned.down_mps_,
        },
    };
    _config.zmq_publish_->Publish(ZMQ_MSG_VEHICLE_PAYLOAD, payload);
}

#pragma endregion private_method

#pragma region signal_slot

void SmallSwarm_Impl::slotZmqMessageArrived(ZMQ::Message& msg_text)
{
    if (msg_text.header_.message_id_ == ZMQ_MSG_VEHICLE_PAYLOAD)
    {
        EventText event_text {
            .metadata_ = msg_text.header_,
            .payload_ = msg_text.payload_,
        };
        {
            std::lock_guard<std::mutex> locker(_event_buffer_lock);
            _event_buffer.push_back(event_text);
        }
    }
}

#pragma endregion signal_slot
