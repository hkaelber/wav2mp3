cmake_minimum_required(VERSION 3.1)

project(wav2mp3 CXX)

SET(WAV2MP3_MAJOR_VERSION 0)
SET(WAV2MP3_MINOR_VERSION 0)
SET(WAV2MP3_PATCH_VERSION 1)

set(WAV2MP3_VERSION ${WAV2MP3_MAJOR_VERSION}.${WAV2MP3_MINOR_VERSION}.${WAV2MP3_PATCH_VERSION})

ADD_DEFINITIONS(-DWAV2MP3_VERSION=\"${WAV2MP3_VERSION}\")

# c++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
        set(my_cxx_flags "-std=c++11")
else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${my_cxx_flags} -Wall")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package (Threads)

find_package(Lame REQUIRED)
include_directories(BEFORE SYSTEM ${LAME_INCLUDE_DIR})

add_subdirectory (src)
