include(ExternalProject)

set(DEPENDENCIES)
set(EXTRA_CMAKE_ARGS -DBUILD_THIRD_PARTY=OFF -DBOOST_ROOT=${BOOST_ROOT})

list(APPEND DEPENDENCIES curl)
ExternalProject_Add(curl
    PREFIX          curl
    GIT_REPOSITORY  https://github.com/curl/curl.git
    GIT_TAG         curl-7_53_1
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                    -DBUILD_CURL_EXE=OFF
                    -DBUILD_TESTING=OFF
                    -DENABLE_MANUAL=OFF
)
ExternalProject_Get_Property(curl install_dir)
# Add the following paths so that FindCURL.cmake can find curl
list(APPEND CMAKE_INCLUDE_PATH ${install_dir}/include)
list(APPEND CMAKE_LIBRARY_PATH ${install_dir}/lib)

list(APPEND DEPENDENCIES SFML)
ExternalProject_Add(SFML
    PREFIX          SFML
    GIT_REPOSITORY  https://github.com/SFML/SFML.git
    GIT_TAG         2.4.2
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
ExternalProject_Get_Property(SFML install_dir)
list(APPEND EXTRA_CMAKE_ARGS -DSFML_ROOT=${install_dir})
set(SFML_ROOT ${install_dir})

list(APPEND DEPENDENCIES TGUI)
ExternalProject_Add(TGUI
    DEPENDS         SFML
    PREFIX          TGUI
    GIT_REPOSITORY  https://github.com/texus/TGUI.git
    GIT_TAG         0.7.4
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                    -DSFML_ROOT=${SFML_ROOT}
)
ExternalProject_Get_Property(TGUI install_dir)
list(APPEND EXTRA_CMAKE_ARGS -DTGUI_ROOT=${install_dir})

ExternalProject_Add(SpeedBlocks
    DEPENDS         ${DEPENDENCIES}
    PREFIX          SpeedBlocks
    SOURCE_DIR      ${PROJECT_SOURCE_DIR}
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                    -DCMAKE_INCLUDE_PATH=${CMAKE_INCLUDE_PATH}
                    -DCMAKE_LIBRARY_PATH=${CMAKE_LIBRARY_PATH}
                    ${EXTRA_CMAKE_ARGS}
    INSTALL_COMMAND ""
    BUILD_ALWAYS    1
)
