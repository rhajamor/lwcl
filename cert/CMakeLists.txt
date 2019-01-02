set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(TARGET_NAME cert)
 

list(APPEND HEADER_DIRS
		"${CMAKE_SOURCE_DIR}/asn_1/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(HEADER_DIRS ${HEADER_DIRS})

#include_directories(${HEADER_DIRS})

list(APPEND SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/source/certificate.cpp" 
)
message("HEADER_DIRS=${HEADER_DIRS}" )
add_executable(${TARGET_NAME}  ${SRC_FILES})
#add_library(${TARGET_NAME} SHARED ${SRC_FILES})
#set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${TARGET_NAME} PUBLIC ${HEADER_DIRS})
target_link_libraries(${TARGET_NAME} asn_1)

install(TARGETS ${TARGET_NAME}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION Debug/bin)
install(TARGETS ${TARGET_NAME}
        CONFIGURATIONS Release
        RUNTIME DESTINATION Release/bin)