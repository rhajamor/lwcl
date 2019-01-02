 
set(TARGET_NAME asn_1)

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:/include/${TARGET_NAME}>
)

install(TARGETS  ${TARGET_NAME})

