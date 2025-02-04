/**
 * @subpage     Checksum Calculator
 * @author      imindude@gmail.com
 * @brief       checksum 계산\n
 *              CCITT 방식의 CRC-8, CRC-16
 */

#pragma once

#include <cstdint>

/**
 * @brief   Checksum 클래시는 static method만 사용
 */
class Checksum
{
public:

    /**
     * @brief  CRC-8-CCITT. 정적 메소드.
     * @param  bytes   입력 바이트 배열의 포인터
     * @param  size    바이트 배열의 원소 수
     * @param  seed    초기값 (기본값은 0x12)
     * @return crc8 체크섬
     */
    static uint8_t crc8ccitt(const uint8_t* bytes, std::size_t size, uint8_t seed = 0x12)
    {
        for (std::size_t n = 0; n < size; n++)
            seed = _crc8ccitt_table[seed ^ bytes[n]];
        return seed;
    }
    /**
     * @brief  CRC-16-CCITT. 정적 메소드.
     * @param  words   입력 워드 배열의 포인터 (당연히 16bit)
     * @param  size    워드 배열의 원소 수
     * @param  seed    초기값 (기본값은 0x1214)
     * @return crc16 체크섬
     */
    static uint16_t crc16ccitt(const uint16_t* words, std::size_t size, uint16_t seed = 0x1214)
    {
        for (std::size_t n = 0; n < size; n++)
            seed = (seed << 8) ^ _crc16ccitt_table[((seed >> 8) ^ words[n]) & 0xFF];
        return seed;
    }

private:

    static uint8_t  _crc8ccitt_table[256];  ///< CRC-8-CCITT 체크섬 테이블
    static uint16_t _crc16ccitt_table[256]; ///< CRC-16-CCITT 체크섬 테이블
};
