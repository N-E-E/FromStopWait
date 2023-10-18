cmake_minimum_required(VERSION 3.10)
project(FromStopWait)

SET(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(stopwait_test app/StopWait.cpp)
add_executable(gbn_test app/GBN.cpp)
add_executable(sr_test app/SR.cpp)
add_executable(tcp_test app/TCP.cpp)

TARGET_LINK_LIBRARIES(stopwait_test stopwait)
TARGET_LINK_LIBRARIES(gbn_test gbn)
TARGET_LINK_LIBRARIES(sr_test sr)
TARGET_LINK_LIBRARIES(tcp_test tcp)