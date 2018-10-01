cmake_minimum_required(VERSION 2.8)

project(MultiThread)

file(GLOB SOURCES src/*.cpp src/*.hpp)
file(GLOB HEADERS include/*.h)

include_directories(./include)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -std=c++11 -O0 -fpic" )
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11 -O2" )

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
   message(STATUS "Setting build type to 'Debug' as none was specified.")
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

add_executable(producer_consumer ${SOURCES} ${HEADERS})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
        add_definitions(-DHAVE_PTHREAD)
	target_compile_options(producer_consumer PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
        add_definitions(-DHAVE_PTHREAD=1)
	target_link_libraries(producer_consumer "${CMAKE_THREAD_LIBS_INIT}")
endif()

add_definitions(-DUSE_PTHREAD=1)



