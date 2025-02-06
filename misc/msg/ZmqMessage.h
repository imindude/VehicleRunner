/**
 * @subpage     ZeroMQ Message
 * @author      imindude@gmail.com
 * @brief       0MQ를 이용한 메세지 교환용 메세지 형태 정의
 */

#pragma once

#include <cstdint>

#define ZMQ_VEHICLE_ID_UNKNOWN    0
#define ZMQ_VEHICLE_ID_LEADER     1
#define ZMQ_VEHICLE_ID_SUB_LEADER 2
#define ZMQ_VEHICLE_ID_MEMBER_NUL 10
#define ZMQ_VEHICLE_ID_MEMBER_1   (ZMQ_VEHICLE_ID_MEMBER_NUL + 1)
#define ZMQ_VEHICLE_ID_MEMBER_2   (ZMQ_VEHICLE_ID_MEMBER_NUL + 2)
#define ZMQ_VEHICLE_ID_MEMBER_3   (ZMQ_VEHICLE_ID_MEMBER_NUL + 3)

#define ZMQ_MSG_UNKNOWN_MESSAGE 0
#define ZMQ_MSG_VEHICLE_PAYLOAD 1

#pragma region zmq_ns
namespace ZMQ
{
/**
 * @brief   0MQ 메세지의 prefix\n
 *          메세지를 발생시키는 주체 및 메세지 종류
 */
#pragma pack(push, 1)
struct Header
{
    uint8_t vehicle_id_ { ZMQ_VEHICLE_ID_UNKNOWN };  ///< vehicle의 식별자 (리더, 서브리더, 멤버)
    uint8_t message_id_ { ZMQ_MSG_UNKNOWN_MESSAGE }; ///< 제공되는 메세지 식별자
};
#pragma pack(pop)

/**
 * @brief   0MQ 메세지의 postfix\n
 *          메세지의 checksum
 */
#pragma pack(push, 1)
struct Footer
{
    uint8_t checksum_; ///< CRC-8-CCITT checksum. Footer를 제외한 전체에 대하여.
};
#pragma pack(pop)

/**
 * @brief   EPSG:4326 좌표계
 */
#pragma pack(push, 1)
struct Position
{
    double latitude_deg_;  ///< 위도
    double longitude_deg_; ///< 경도
    float  altitude_m_;    ///< 고도 (RTK base를 0으로 하는 고도 정보. RTK base는 리더가 제공)
};
#pragma pack(pop)

/**
 * @brief   NED좌표계
 */
#pragma pack(push, 1)
struct Velocity
{
    float north_mps_; ///< 북쪽 성분의 m/s
    float east_mps_;  ///< 동쪽 성분의 m/s
    float down_mps_;  ///< 아래쪽(지구중심) 성분의 m/s
};
#pragma pack(pop)

/**
 * @brief   payload의 내용이 많지 않은 상태에서 메세지를 다수로 쪼개는 것이 별로인 듯하여 하나로 뭉쳐놓음
 */
#pragma pack(push, 1)
struct Payload
{
    Position position_; ///< EPSG:4326 좌표계의 경위도 및 고도
    Velocity velocity_; ///< NED 좌표계로 표현되는 vehicle의 속도 (m/s)
};
#pragma pack(pop)

/**
 * @brief   0MQ 메세지를 간단하게 핸들링하기 위한 구조체/공용체 정의
 */
#pragma pack(push, 1)
struct Message
{
    Header  header_;  ///< 메세지 식별자
    Payload payload_; ///< 메세지 내용
    Footer  footer_;  ///< 메세지 체크섬
};
#pragma pack(pop)

}
#pragma endregion zmq_ns
