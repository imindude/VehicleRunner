/**
 * @subpage     MAVLink Message
 * @author      imindude@gmail.com
 * @brief       MAVLink V2 Extension의 사용자 정의 메세지 (header only)
 */

#pragma once

#include <mavsdk/mavlink/common/mavlink.h>

#define MAVMSG_NET_ID_BROADCAST 0
#define MAVMSG_SYS_ID_BROADCAST 0
#define MAVMSG_COMP_ID_GCB      MAV_COMP_ID_PAIRING_MANAGER ///< mavlink를 사용하는 지상(관제) 도우미
#define MAVMSG_COMP_ID_FCS      MAV_COMP_ID_PATHPLANNER     ///< onboard computer (mission computer)
#define MAVMSG_COMP_ID_GCS      MAV_COMP_ID_MISSIONPLANNER  ///< GCS (QGC등)

#define MAVMSG_V2EXT_FEATURE 40001

#pragma region mav_namespace
namespace MAV
{
/**
 * @brief   MAVMSG_V2EXT_FEATURE 메세지 처리
 */
#pragma pack(push, 1)
struct Feature
{
    bool log_capture_ { false };      ///< 외부 메모리에 로그 저장 (암호화 포함)
    bool volatile_mission_ { false }; ///< 미션비행 정보를 SD카드에 저장하지 않음
};
#pragma pack(pop)

/**
 * @brief   MAVLink 메세지를 간단하게 핸들링하기 위한 구조체/공용체 정의
 */
#pragma pack(push, 1)
struct Packet
{
    union Payload
    {
        Feature feature_; ///< vehicle의 활성화 기능
    } payload_;
};
#pragma pack(pop)
}
#pragma endregion zmq_ns
