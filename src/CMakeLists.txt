include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB stopwait_src_list "StopWait/*.cpp")
add_executable(stopwait ${stopwait_src_list})

file(GLOB gbn_src_list "GBN/*.cpp")
add_executable(gbn ${gbn_src_list} Helpers.cpp)

file(GLOB sr_src_list "SR/*.cpp")
add_executable(sr ${sr_src_list} Helpers.cpp)

file(GLOB tcp_src_list "TCP/*.cpp")
add_executable(tcp ${tcp_src_list} GBN/GBNRdtSender.cpp GBN/GBNRdtReceiver.cpp Helpers.cpp)

FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)

TARGET_LINK_LIBRARIES(stopwait  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(gbn  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(sr  ${NETSIM_LIB})
TARGET_LINK_LIBRARIES(tcp  ${NETSIM_LIB})