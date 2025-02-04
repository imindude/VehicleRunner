/**
 * @subpage     MQTT Message
 * @author      imindude@gmail.com
 * @brief       MQTT를 이용한 메세지 교환용 메세지 형태 정의
 */

#pragma once

#include <iostream>

#define MQTT_SOURCE_ID_UNKNOWN 0

#define MQTT_MSG_UNKNOWN_MESSAGE 0

#pragma region mqtt_ns
namespace MQTT
{
    static constexpr auto TOPIC_TEXT_MESSAGE = "text_message";  // std::string::c_str()

/**
 * @brief   테스트용 메세지
 */
#pragma pack(push, 1)
struct TextMessage
{
#define TEXT_MESSAGE_LENGTH 128
    char message_[TEXT_MESSAGE_LENGTH];
};
#pragma pack(pop)
}
#pragma endregion mqtt_ns
