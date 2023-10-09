cmake_minimum_required(VERSION 3.24.1)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
    controller/controller.hpp
    controller/robot_controller.hpp
    web/dto/status_dto.hpp
    web/dto/robot_dto.hpp
    web/app_component.hpp
    service/robot/robot_service.cpp
    processor/avprocessor.cpp
)

set(VCPKG_INCLUDE_DIR ${VCPKG_DIR}/include)
set(VCPKG_LIB_DIR ${VCPKG_DIR}/lib)
set(OATPP_DIR ${VCPKG_INCLUDE_DIR}/oatpp-1.3.0)
set(OATPP_INCLUDE_DIR ${OATPP_DIR}/oatpp/)
set(OATPP_SWAGGER_INCLUDE_DIR ${OATPP_DIR}/oatpp-swagger/)
set(OATPP_LIB_DIR ${VCPKG_LIB_DIR}/oatpp-1.3.0/)
set(FMT_INCLUDE_DIR ${VCPKG_LIB_DIR}/include/fmt)
#set(FFMPEG_INCLUDE_DIR ${VCPKG_INCLUDE_DIR}/libavutil ${VCPKG_INCLUDE_DIR}/libavcodec ${VCPKG_INCLUDE_DIR}/libavformat)

add_definitions(
    -DSERVER_NAME="localhost"
    -DSERVER_ADDRESS="http://localhost"
    -DSERVER_PORT=12000
    -DOATPP_SWAGGER_RES_PATH="${OATPP_BASE_DIR}/bin/oatpp-swagger/res"
    -DDEVELOPER_NAME="BLIPShare"
    -DDEVELOPER_URL="https://www.blipshare.com",
    -DVS1_STREAM_URL="rtsp://alarm:alarm@192.168.0.121:8554/live"
)

add_library(vision_service_lib SHARED main.cpp)

target_compile_options(vision_service_lib
    PRIVATE
    -g -Wall -Wconversion -Wsign-conversion -Wsign-compare
)

target_include_directories(vision_service_lib
    PUBLIC
    ${OATPP_INCLUDE_DIR}
    ${OATPP_SWAGGER_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${VCPKG_INCLUDE_DIR}
)

add_executable(vision_service main ${SOURCE_FILES})

target_link_libraries(vision_service LINK_PUBLIC vision_service_lib)

target_link_directories(vision_service
    PRIVATE
    ${OATPP_LIB_DIR}
    ${VCPKG_LIB_DIR})

target_link_libraries(vision_service
    PUBLIC
    fmt
    oatpp::oatpp
    oatpp::oatpp-swagger
    avcodec
    avformat
    avfilter
    avdevice
    pkgconf
    swscale
    swresample
    avutil
    Threads::Threads
)

# DEPLOYMENT
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/deploy")

set_target_properties(vision_service PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/deploy
)

# Copy the Swagger files
file(COPY 
    ${OATPP_BASE_DIR}/bin/oatpp-swagger/res
    DESTINATION
    ${CMAKE_SOURCE_DIR}/deploy
)