cmake_minimum_required(VERSION 3.24.1)
project(CVService VERSION 1.0.0 LANGUAGES CXX C)

set (/usr/local/lib)

set(VCPKG_DIR /app/vcpkg/installed/arm-linux)

find_package(fmt REQUIRED)
find_package(Threads REQUIRED)
find_package(oatpp 1.3.0 CONFIG PATHS ${VCPKG_DIR} REQUIRED)
find_package(oatpp-swagger 1.3.0 CONFIG PATHS ${VCPKG_DIR} REQUIRED)

add_subdirectory(src)