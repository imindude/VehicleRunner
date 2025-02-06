/**
 * @subpage     MAVLink Vehicle
 * @author      imindude@gmail.com
 * @brief       MAVLink를 이용한 Vehicle의 모니터링 및 제어\n
 *              MAVSDK 사용
 */

#pragma once

#include "MavMessage.h"
#include <boost/signals2.hpp>

class MavVehicle_Impl;

/**
 * @brief   MAVLink/MAVSDK를 이용하여 vehicle과의 정보 교환 및 제어
 */
class MavVehicle
{
#pragma region method_member

public:

    struct PositionLlh
    {
        double latitude_deg_ { 0. };
        double longitude_deg_ { 0. };
        float  msl_altitude_m_ { 0.f };
        float  rel_altitude_m_ { 0.f };
    };

    struct VelocityNed
    {
        float north_mps_ { 0.f };
        float east_mps_ { 0.f };
        float down_mps_ { 0.f };
    };

    struct Config
    {
        std::string connect_url_; ///< ex) "udp://:14550"
        bool        log_wo_sd_;   ///< 비행로그를 SD카드 없이 외부 메모리(USB)에 저장
        bool        plan_wo_sd_;  ///< 미션경로를 RAM에 저장
    };

    explicit MavVehicle(Config& config);
    ~MavVehicle() = default;

    int  Exec();
    void Stop();
    /**
     * @brief   MAVLink V2 Extension 타입의 사용자 정의 메세지 전송
     * @param   target_cid  target component id ("MavMessage.h" 참고)
     * @param   msg_type    message type ("MavMessage.h" 참고)
     * @param   msg_text    사용자 정의 메세지
     * @return  0:success, other:failed
     */
    int PublishMessage(uint8_t target_cid, uint16_t msg_type, MAV::Message& msg_text);
    /**
     * @brief   EPSG:4326 좌표계의 경위도(degree) 및 고도(meter)\n
     *          고도는 MSL고도와, 이륙지점을 기준으로 하는 상대 고도
     * @param   llh     vehicle 3차원 위치 정보 수신 버퍼
     * @return  0:success, other:failed
     */
    int  GetPosition(PositionLlh& llh);
    /**
     * @brief   NED 좌표계로 표현되는 vehicle의 속도 (m/s)
     * @param   ned     vehicle 3차원 속도 정보 수신 버퍼
     * @return  0:success, other:failed
     */
    int  GetVelocity(VelocityNed& ned);

private:

    std::shared_ptr<MavVehicle_Impl> _impl; ///< MavVehicle 클래스의 implementation

#pragma endregion method_member

#pragma region signal_slot

public:

    /**
     * @brief   mavlink를 통한 연결 설정 및 해제 이벤트
     * @ingroup signals
     * @param   _1  connect or not
     */
    using SigConnectionChanged = boost::signals2::signal<void(bool)>;
    /**
     * @brief   mavlink를 이용한 비행로그(uLog)의 수신 처리
     * @ingroup signals
     * @param   _1  uLog 포맷의 바이트열
     */
    using SigLogStreamingArrived = boost::signals2::signal<void(std::vector<uint8_t>&)>;
    /**
     * @brief   mavlink_v2_extension 메세지 수신 이벤트 (커스텀 메세지 처리 용도)
     * @ingroup signals
     * @param   _1  message type (MAVMSG_V2EXT_xxx in "MavMessage.h")
     * @param   _2  message payload (union MavV2Ext in "MavMessage.h")
     */
    using SigExtensionMessageArrived = boost::signals2::signal<void(uint16_t, std::basic_string<uint8_t>&)>;

    /**
     * @ref     SigConnectionChanged
     * @ingroup signals
     * @return  "SigConnectionChanged" 핸들 리턴
     */
    SigConnectionChanged& SigConnectionChangedHandle();
    /**
     * @ref     SigLogStreamingArrived
     * @ingroup signals
     * @return  "SigLogStreamingArrived" 핸들 리턴
     */
    SigLogStreamingArrived& SigLogStreamingArrivedHandle();
    /**
     * @ref     SigExtensionMessageArrived
     * @ingroup signals
     * @return  "SigExtensionMessageArrived" 핸들 리턴
     */
    SigExtensionMessageArrived& SigExtensionMessageArrivedHandle();

private:

#pragma endregion signal_slot
};
