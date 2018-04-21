cmake_minimum_required (VERSION 3.0)
project (IMAP_SMTP_CLIENT)

set(CMAKE_BUILD_TYPE Release)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(client ${SOURCES})