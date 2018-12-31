cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TARGET_NAME lwcl)
project(${TARGET_NAME})

add_subdirectory(asn.1)
add_subdirectory(cert)

