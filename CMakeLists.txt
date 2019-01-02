cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TARGET_NAME lwcl)
project(${TARGET_NAME})
  

add_subdirectory(asn_1)
add_subdirectory(cert)

