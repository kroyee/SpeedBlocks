cmake_minimum_required(VERSION 3.5)

project(SpeedBlocks)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic")

set(SOURCE_FILES
        EmptyResourcePath.cpp
        EmptyResourcePath.h
        gameField.cpp
        gameField.h
        gameFieldDrawer.cpp
        gamePlay.cpp
        gamePlay.h
        gui.cpp
        gui.h
        main.cpp
        MingwConvert.cpp
        MingwConvert.h
        network.cpp
        network.h
        optionSet.cpp
        optionSet.h
        packetcompress.cpp
        packetcompress.h
        pieces.cpp
        pieces.h
        randomizer.cpp
        randomizer.h
        ResourcePath.hpp
        sounds.cpp
        sounds.h
        textures.cpp
        textures.h)

add_executable(SpeedBlocks ${SOURCE_FILES})

find_package(SFML 2 COMPONENTS audio network graphics window system REQUIRED)

find_package(TGUI 0.7.3 REQUIRED)

target_link_libraries(SpeedBlocks ${SFML_LIBRARIES} ${TGUI_LIBRARY})

if (NOT ${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/media DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()