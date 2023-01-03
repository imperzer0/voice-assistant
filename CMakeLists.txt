cmake_minimum_required(VERSION 3.24)
project(voice-assistant)

find_package(cpprestsdk REQUIRED)

find_package(SFML 2.5 COMPONENTS system audio REQUIRED)

set(CMAKE_CXX_STANDARD 20)

if (DEFINED PACKAGE_VERSION)
    add_compile_definitions("VERSION=\"${PACKAGE_VERSION}\"")
endif ()

if (DEFINED APPNAME)
    add_compile_definitions("APPNAME=\"${APPNAME}\"")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -lX11")

if (NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    message("Disabled optimization flags for use in profiler.")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
    message("Enabled optimization compiler flags.")
endif ()

add_executable(voice-assistant main.cpp cmdline.cpp config.cpp assistant.cpp graphics.cpp)

target_link_libraries(voice-assistant cpprestsdk::cpprest crypto ssl pthread sfml-system sfml-audio)