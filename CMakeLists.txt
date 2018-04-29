cmake_minimum_required (VERSION 3.1)
project (IMAP_SMTP_CLIENT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(client ${SOURCES})

find_package(OpenSSL 1.0.2 REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(client ${OPENSSL_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

# add_library(stdc++fs UNKNOWN IMPORTED)
# set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")

# target_link_libraries(client stdc++fs)
target_link_libraries(client readline)

FIND_LIBRARY(CPPEXPERIMENTAL_LIBRARY
             NAMES stdc++fs
             PATHS ${LIBCPPEXPERIMENTAL}/lib/
                    /usr/lib/gcc/x86_64-linux-gnu/5
                    /usr/lib/gcc/x86_64-linux-gnu/6
                    /usr/lib/gcc/x86_64-linux-gnu/7
                    /usr/lib/x86_64-linux-gnu/
                    /usr/local/lib64/
                    /usr/lib64/
                    /usr/lib/)
MARK_AS_ADVANCED(CPPEXPERIMENTAL_LIBRARY)

###########################################################################
IF (CPPEXPERIMENTAL_LIBRARY)
    SET(CPPEXPERIMENTAL_FOUND 1)
    SET(CPPEXPERIMENTAL_LIBRARIES ${CPPEXPERIMENTAL_LIBRARY})
ENDIF()

MARK_AS_ADVANCED(CPPEXPERIMENTAL_LIBRARIES)

IF (CPPEXPERIMENTAL_FOUND)
    MESSAGE(STATUS "Found libstdc++fs: ${CPPEXPERIMENTAL_LIBRARIES}")
ELSE ()
    MESSAGE(STATUS "Could not find libstdc++fs.")
ENDIF()
target_link_libraries(client ${CPPEXPERIMENTAL_LIBRARY})

