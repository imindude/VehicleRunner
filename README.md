# Vehicle Runner

- MAVLink를 이용한 vehicle 제어 : PX4를 사용하는 Auto Pilot에 대응
- ZeroMQ를 이용하여 vehicle간 통신 예상
  - MAVLink를 이용할 경우, 중간에 통신을 중계해 줄 broker가 필요할 것
  - 각 노드(vehicle)들이 임의로 추가/삭제 될 것을 고려할 경우, broker의 존재는 곤란할 수도...
- MQTT를 이용하여 vehicle과 station간 통신(을 할 수도 있으나, MAVLink를 이용하는 것으로...)

## 적용 시스템

- posix 호환 OS
- debian12에서 확인 : WSL debian 및 RPi4/5 rasbian OS lite version

## 적용 library

| 이름 | 라이브러리 | 버전 | 참고 |
| :-- | :-- | :-- | :-- |
| boost | libboost-dev | 1.74.0.3 | |
| PahoMqttCpp | libpaho-mqttpp-dev | 1.2.0-2 | 참고용 |
| ZeroMQ | libzmq5-dev | 4.3.4-6 | vehicle간 통신 |
| MessagePack | libmsgpack-cxx-dev | 4.1.3-1 | 참고용 |
| JsonCpp | libjsoncpp-dev | 1.9.5-4 | |
| MAVSDK | libmavsdk-dev | 2.12.12 | |
