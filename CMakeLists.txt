cmake_minimum_required(VERSION 3.3)
project(https_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    main.c
    https.c
    https.h)

include_directories(.)
include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/../mbedtls-2.2.1/include)

add_executable(https_client ${SOURCE_FILES})

target_link_libraries(https_client ${CMAKE_SOURCE_DIR}/../mbedtls-2.2.1/library/libmbedx509.a)
target_link_libraries(https_client ${CMAKE_SOURCE_DIR}/../mbedtls-2.2.1/library/libmbedtls.a)
target_link_libraries(https_client ${CMAKE_SOURCE_DIR}/../mbedtls-2.2.1/library/libmbedcrypto.a)
target_link_libraries(https_client pthread)
