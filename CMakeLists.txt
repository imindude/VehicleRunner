## ---------------------------------------------------------------------------------------------------------------------
## @author  imindude@gmail.com
## ---------------------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20.0)
project(VehicleRunner VERSION 0.0.1 LANGUAGES C CXX)
configure_file(${PROJECT_SOURCE_DIR}/version.h.in ${PROJECT_BINARY_DIR}/version.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_BUILD_TYPE Debug)

message(STATUS "Compiler  : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "> Version : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "> Path    : ${CMAKE_CXX_COMPILER}")

if(WIN32)
    macro(get_WIN32_WINNT version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
    # set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    add_compile_options(-Wall -Wextra -Werror -Wno-unknown-pragmas)
    # add_compile_options(-Wall -Wextra)
endif()

## ---------------------------------------------------------------------------------------------------------------------

find_package(Boost REQUIRED COMPONENTS system thread chrono)
find_package(PahoMqttCpp REQUIRED)
# find_package(ZeroMQ REQUIRED)
# find_package(msgpack)
find_package(jsoncpp REQUIRED)
find_package(MAVSDK REQUIRED)

if(NOT Boost_LIBRARIES)
    set(Boost_LIBRARIES boost_thread)
endif()
if(NOT PahoMqttCpp_LIBRARIES)
    set(PahoMqttCpp_LIBRARIES PahoMqttCpp::paho-mqttpp3)
endif()
if(NOT ZeroMQ_LIBRARIES)
    set(ZeroMQ_LIBRARIES zmq)
endif()
if(NOT jsoncpp_LIBRARIES)
    set(jsoncpp_LIBRARIES JsonCpp::JsonCpp)
endif()
if(NOT MAVSDK_LIBRARIES)
    set(MAVSDK_LIBRARIES MAVSDK::mavsdk)
endif()

include_directories(
    ${CMAKE_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${PahoMqttCpp_INCLUDE_DIRS}
    ${ZeroMQ_INCLUDE_DIRS}
    ${msgpack_INCLUDE_DIRS}
    ${jsoncpp_INCLUDE_DIRS}
    ${MAVSDK_INCLUDE_DIRS}
)

link_libraries(
    ${Boost_LIBRARIES}
    ${PahoMqttCpp_LIBRARIES}
    ${ZeroMQ_LIBRARIES}
    ${jsoncpp_LIBRARIES}
    ${MAVSDK_LIBRARIES}
)

## ---------------------------------------------------------------------------------------------------------------------

add_subdirectory(app)
add_subdirectory(plugin)
add_subdirectory(bundle)
add_subdirectory(misc)
# add_subdirectory(addon)

add_dependencies(app plugin)
add_dependencies(plugin bundle)
add_dependencies(bundle misc)

## ---------------------------------------------------------------------------------------------------------------------

add_executable(
    ${PROJECT_NAME}
        Main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
        app
        plugin
        bundle
        misc
        # erkir
)

file(COPY ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.json DESTINATION ${PROJECT_BINARY_DIR}/)

## EOF -----------------------------------------------------------------------------------------------------------------
