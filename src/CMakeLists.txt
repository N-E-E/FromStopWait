include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB stopwait_src_list "StopWait/*.cpp")
add_library(stopwait ${stopwait_src_list})

file(GLOB gbn_src_list "GBN/*.cpp")
add_library(gbn ${gbn_src_list} Helpers.cpp)

file(GLOB sr_src_list "SR/*.cpp")
add_library(sr ${sr_src_list} Helpers.cpp)

file(GLOB tcp_src_list "TCP/*.cpp")
add_library(tcp ${tcp_src_list} ${gbn_src_list} Helpers.cpp)

FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)

TARGET_LINK_LIBRARIES(stopwait  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(gbn  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(sr  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(tcp  ${NETSIM_LIB})