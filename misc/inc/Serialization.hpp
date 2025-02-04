/**
 * @subpage     Serializer
 * @author      imindude@gmail.com
 * @brief       네트워크 전송, 저장등을 위한 데이터 serialization\n
 *              MessagePack 라이브러리 이용
 * @section     사용법
 * @code
 *              struct Head {
 *                  float vid_;
 *                  float pid_;
 *                  MSGPACK_DEFINE(vid_, pid_);
 *              };
 *              struct Foot {
 *                  float chk_;
 *                  MSGPACK_DEFINE(chk_);
 *              };
 *
 *              struct WhatPack {
 *                  Head header_;
 *                  Foot footer_;
 *                  MSGPACK_DEFINE(header_, footer_);
 *              };
 *
 *              std::basic_string<uint8_t> container;
 *              WhatPack org {
 *                  .header_ {
 *                      .vid_ = 123.3214,
 *                      .pid_ = 123.123123,
 *                  },
 *                  .footer_ {
 *                      .chk_ = 0,
 *                  }
 *              };
 *              WhatPack msg;
 *
 *              Serialization<WhatPack>::pack(org, container);
 *              Serialization<WhatPack>::unpack(container, msg);
 * @endcode
 */

#pragma once

#include <cstdint>
#include <msgpack.hpp>

/**
 * @brief   메세지 serialization / deserialization\n
 *          MessagePack 라이브러리 사용
 */
template <typename T>
class Serialization
{
public:

    /**
     * @brief  MessagePack을 이용한 메세지 packer
     * @param  message      메세지 원본
     * @param  container    serialization한 메세지의 저장소
     */
    static void pack(T& message, std::basic_string<uint8_t>& container)
    {
        std::stringstream ss;
        msgpack::pack(ss, message);

        container.resize(ss.str().size(), 0);
        std::memcpy(container.data(), ss.str().data(), container.size());
    }
    /**
     * @brief  MessagePack을 이용한 메세지 unpacker
     * @param  container    serialization되어 있는 메세지 원본 (수신 또는 읽어들인 메세지)
     * @param  message      deserialization한 메세지 저장소
     */
    static void unpack(std::basic_string<uint8_t>& container, T& message)
    {
        message = msgpack::unpack(reinterpret_cast<char*>(container.data()), container.size()).get().as<T>();
    }
};
