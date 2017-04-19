cmake_minimum_required(VERSION 3.5)

option(BUILD_THIRD_PARTY "Download and build third party libraries" OFF)

if(BUILD_THIRD_PARTY)
    project(All)
    include(third_party/CMakeLists.txt)
    return()
endif()

project(SpeedBlocks)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
add_definitions(-DSFML_NO_DEPRECATED_WARNINGS)

set(EXECUTABLE_NAME SpeedBlocks)
file(GLOB SOURCE_FILES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.h  ${CMAKE_SOURCE_DIR}/*.hpp)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

add_custom_target(run
    COMMAND ${EXECUTABLE_NAME}
    DEPENDS SpeedBlocks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

find_package(SFML 2 COMPONENTS audio network graphics window system REQUIRED)
find_package(TGUI 0.7.3 REQUIRED)
find_package(Boost REQUIRED)
find_package(CURL REQUIRED)

include_directories(${SFML_INCLUDE_DIR} ${TGUI_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${TGUI_LIBRARY} ${CURL_LIBRARY})

if (NOT ${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/media DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()
