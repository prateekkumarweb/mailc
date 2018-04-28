cmake_minimum_required (VERSION 3.1)
project (IMAP_SMTP_CLIENT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Release)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(client ${SOURCES})

find_package(OpenSSL 1.0.2 REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(client ${OPENSSL_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

add_library(stdc++fs UNKNOWN IMPORTED)
set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")

target_link_libraries(client stdc++fs)