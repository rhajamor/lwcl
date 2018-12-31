 
set(TARGET_NAME cert)
list(APPEND HEADER_FILES
        include/
)
list(APPEND SRC_FILES 
    source/*.cpp
)
add_library(${TARGET_NAME} SHARED ${SRC_FILES})
target_include_directories(${TARGET_NAME} ${HEADER_FILES})