SET(pwd ${CMAKE_CURRENT_LIST_DIR})

SET(src "")
AUX_SOURCE_DIRECTORY(${pwd} src)
AUX_SOURCE_DIRECTORY(${pwd}/minecraft src)
AUX_SOURCE_DIRECTORY(${pwd}/fluvit src)
add_executable(fluvit ${src})
message(STATUS "fluvit added ${src}")

target_link_libraries(fluvit ${stdgl_libraries})
FIND_PACKAGE(JPEG REQUIRED)
TARGET_LINK_LIBRARIES(fluvit ${JPEG_LIBRARIES})
