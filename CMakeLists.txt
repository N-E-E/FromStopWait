cmake_minimum_required(VERSION 3.10)
project(FromStopWait)

SET(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

set(STOPWAIT_SRC_LIST 
    src/StopWait.cpp 
    src/StopWaitRdtReceiver.cpp 
    src/StopWaitRdtSender.cpp)

set(GBN_SRC_LIST 
    src/GBN.cpp 
    src/GBNRdtReceiver.cpp 
    src/GBNRdtSender.cpp
    src/Helpers.cpp)

set(SR_SRC_LIST 
    src/SR.cpp 
    src/SRRdtReceiver.cpp 
    src/SRRdtSender.cpp
    src/Helpers.cpp)

set(TCP_SRC_LIST 
    src/TCP.cpp 
    src/TCPReceiver.cpp 
    src/TCPSender.cpp
    src/GBNRdtReceiver.cpp 
    src/GBNRdtSender.cpp
    src/Helpers.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(stop_wait ${STOPWAIT_SRC_LIST})
add_executable(gbn ${GBN_SRC_LIST})
add_executable(sr ${SR_SRC_LIST})
add_executable(tcp ${TCP_SRC_LIST})

FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)

TARGET_LINK_LIBRARIES(stop_wait  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(gbn  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(sr  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(tcp  ${NETSIM_LIB})