cmake_minimum_required (VERSION 3.0)
project (IMAP_SMTP_CLIENT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(client ${SOURCES})

find_package(OpenSSL 1.1.1 REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(client ${OPENSSL_LIBRARIES})