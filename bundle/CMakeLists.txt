##
## @author  imindude@gmail.com
##

set(TARGET bundle)

set(
    SOURCES
        # -- interface
        MavVehicle.cpp
        ZmqPublish.cpp
        ZmqSubscribe.cpp
        MqttClient.cpp
        # -- implementation
        impl/MavVehicle_Impl.cpp
        impl/ZmqPublish_Impl.cpp
        impl/ZmqSubscribe_Impl.cpp
        impl/MqttClient_Impl.cpp
)

add_library(
    ${TARGET}
        OBJECT
            ${SOURCES}
)

target_include_directories(
    ${TARGET}
        PUBLIC
            inc
        PRIVATE
            impl
)

target_link_libraries(
    ${TARGET}
        PUBLIC
            misc
)

## EOF
