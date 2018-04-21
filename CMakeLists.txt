cmake_minimum_required (VERSION 3.0)
project (IMAP_SMTP_CLIENT)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(client ${SOURCES})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(client ${OPENSSL_LIBRARIES})