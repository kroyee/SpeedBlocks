cmake_minimum_required(VERSION 3.12)

option(BUILD_THIRD_PARTY "Download and build third party libraries" OFF)

if(BUILD_THIRD_PARTY)
    project(All)
    include(third_party/CMakeLists.txt)
    return()
endif()

project(SpeedBlocks)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
add_definitions(-DSFML_NO_DEPRECATED_WARNINGS)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/lib/:$$ORIGIN/lib/")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -static -lpthread")
endif()

set(EXECUTABLE_NAME SpeedBlocks)
file(GLOB SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h  ${CMAKE_SOURCE_DIR}/src/gui/*.cpp ${CMAKE_SOURCE_DIR}/src/gui/*.h  ${CMAKE_SOURCE_DIR}/src/resources/*.cpp ${CMAKE_SOURCE_DIR}/src/resources/*.h  ${CMAKE_SOURCE_DIR}/src/gameplay/*.cpp ${CMAKE_SOURCE_DIR}/src/gameplay/*.h ${CMAKE_SOURCE_DIR}/src/utils/*.hpp ${CMAKE_SOURCE_DIR}/src/utils/*.mm ${CMAKE_SOURCE_DIR}/src/utils/*.h ${CMAKE_SOURCE_DIR}/src/utils/*.cpp)
if(APPLE)
    list(REMOVE_ITEM SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/utils/EmptyResourcePath.h)
    list(REMOVE_ITEM SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/utils/EmptyResourcePath.cpp)
    set(MACOSX_BUNDLE_ICON_FILE media/speedblocks.icns)
else()
    list(REMOVE_ITEM SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/utils/ResourcePath.hpp)
    list(REMOVE_ITEM SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/utils/ResourcePath.mm)
    list(REMOVE_ITEM SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/utils/RunAsAdmin.hpp)
    list(REMOVE_ITEM SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/utils/RunAsAdmin.mm)
endif()
if(WIN32)
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE WIN32 ${SOURCE_FILES} SpeedBlocks.rc)
else()
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE WIN32 ${SOURCE_FILES})
endif()

target_include_directories(SpeedBlocks PUBLIC ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/resources ${CMAKE_SOURCE_DIR}/src/gameplay ${CMAKE_SOURCE_DIR}/src/utils)

add_custom_target(run
    COMMAND ${EXECUTABLE_NAME}
    DEPENDS SpeedBlocks
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

find_package(SFML 2 COMPONENTS audio network graphics window system)
if(NOT SFML_FOUND)
    set(SFML_ROOT "" CACHE PATH "SFML root directory")
    message(FATAL_ERROR "CMake couldn't find SFML. Set the SFML_ROOT entry to SFML's root directory (containing \"include\" and \"lib\" directories).")
endif()
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

find_package(TGUI 0.8)
if(NOT TGUI_FOUND)
    set(TGUI_ROOT "" CACHE PATH "TGUI root directory")
    message(FATAL_ERROR "CMake couldn't find TGUI. Set the TGUI_ROOT entry to TGUI's root directory (containing \"include\" and \"lib\" directories).")
endif()
include_directories(${TGUI_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${TGUI_LIBRARY})

find_package(curl)
if(NOT CURL_FOUND)
    if(NOT CURL_INCLUDE_DIR)
        set(CURL_INCLUDE_DIR "" CACHE PATH "Curl include directory")
        message(FATAL_ERROR "CMake couldn't find Curl. Set CURL_INCLUDE_DIR to Curl's \"include\" directory.")
    endif()
    if(NOT CURL_LIBRARY)
        set(CURL_LIBRARY "" CACHE PATH "Curl link library file path")
        message(FATAL_ERROR "CMake couldn't find Curl. Set CURL_LIBRARY to Curl's link library file path.")
    endif()
endif()
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${CURL_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries (${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})

if (WIN32)
    target_link_libraries(${EXECUTABLE_NAME} iphlpapi.lib)
endif()

if (APPLE)
    set(BUNDLE_PATH ${CMAKE_CURRENT_BINARY_DIR}/Debug/SpeedBlocks.app)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/media DESTINATION ${BUNDLE_PATH}/Contents/Resources)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/sounds DESTINATION ${BUNDLE_PATH}/Contents/Resources)
    get_filename_component(SFML_LIBRARY_DIR ${SFML_SYSTEM_LIBRARY} DIRECTORY)
    set(LIBRARIES_DIR ${SFML_LIBRARY_DIR})
    # Copy freetype manually, since it otherwise just causes problems (I think because it internally uses
    # @executable_path instead of @rpath)
    install(DIRECTORY ${SFML_LIBRARY_DIR}/freetype.framework DESTINATION ${BUNDLE_PATH}/Contents/Frameworks)
    # Let BundleUtilities do everything else automatically
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${BUNDLE_PATH}\" \"\" \"${LIBRARIES_DIR}\")
        " COMPONENT Runtime
    )
elseif(NOT ${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/media DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (${EXECUTABLE_NAME} X11)
    install(TARGETS SpeedBlocks DESTINATION ${PROJECT_SOURCE_DIR}/build)
endif()