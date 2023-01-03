INCLUDE(ExternalProject)
FIND_PACKAGE(Git REQUIRED)

SET(JSON_PACKAGE_NAME "cisson")
SET(SG_PACKAGE_NAME "libsagui")
SET(EV_PACKAGE_NAME "libevent")

# complie demo.c
ExternalProject_Add(
    ${JSON_PACKAGE_NAME}
    PREFIX ${CMAKE_BINARY_DIR}/demo/cisson
    GIT_REPOSITORY https://gitlab.com/Meithal/cisson.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(${JSON_PACKAGE_NAME} SOURCE_DIR)
SET(JSON_INCLUDE_DIR ${SOURCE_DIR}/ CACHE INTERNAL "Path to include folder for json package")

FILE(GLOB_RECURSE srcs demo.c src/*.c)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


SET(DEMO_NAME "demo")

ADD_EXECUTABLE(${DEMO_NAME} ${srcs})

TARGET_LINK_LIBRARIES(${DEMO_NAME} PUBLIC ${PROJECT_NAME})
TARGET_INCLUDE_DIRECTORIES(${DEMO_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/demo/inc ${JSON_INCLUDE_DIR})

# test demo
FILE(GLOB examples "${CMAKE_CURRENT_BINARY_DIR}/example/*.json")
FOREACH(file ${examples})
    ADD_TEST(NAME demo_${file} COMMAND ${DEMO_NAME} ${file})
ENDFOREACH()

# complie apiserver.c
FILE(GLOB_RECURSE srcs apiserver.c src/*.c)

SET(API_SERVER_NAME "apiserver")

# checking libsagui exist or not. If not exist, compile from source
FIND_LIBRARY(sagui NAMES sagui)
IF(${sagui} STREQUAL "sagui-NOTFOUND")
    MESSAGE(STATUS "no libsagui, compile from source code")
    SET(SG_VERSION "3.4.0")
    STRING(SUBSTRING ${SG_VERSION} 0 1 SG_MAJOR)

    ExternalProject_Add(
        ${SG_PACKAGE_NAME}
        PREFIX ${CMAKE_BINARY_DIR}/demo/sagui
        GIT_REPOSITORY https://github.com/risoflora/libsagui.git
        GIT_TAG v${SG_VERSION}
        GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
        TIMEOUT 10
        CMAKE_ARGS -DBUILD_TESTING=ON
        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} package
        LOG_DOWNLOAD ON
    )

    ExternalProject_Get_Property(${SG_PACKAGE_NAME} SOURCE_DIR)
    SET(SG_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include folder for libsagui package")
    ExternalProject_Get_Property(${SG_PACKAGE_NAME} BINARY_DIR)

    SET(SG_LIBRARY_NAME "${CMAKE_SHARED_MODULE_PREFIX}sagui${CMAKE_SHARED_LIBRARY_SUFFIX}")

    ExternalProject_Add_Step(
        ${SG_PACKAGE_NAME} copy2lib
        COMMAND ${CMAKE_COMMAND} -E tar cf sagui.tar ${SG_LIBRARY_NAME} ${SG_LIBRARY_NAME}.${SG_VERSION} ${SG_LIBRARY_NAME}.${SG_MAJOR}
        COMMAND ${CMAKE_COMMAND} -E copy sagui.tar ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${BINARY_DIR}/src/
        DEPENDEES install
    )
    ExternalProject_Add_Step(
        ${SG_PACKAGE_NAME} extract
        COMMAND ${CMAKE_COMMAND} -E tar xf sagui.tar
        COMMAND ${CMAKE_COMMAND} -E rm sagui.tar
        WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        DEPENDEES copy2lib
    )

    ADD_LIBRARY(sagui SHARED IMPORTED)
    SET_PROPERTY(TARGET sagui PROPERTY IMPORTED_LOCATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${SG_LIBRARY_NAME})
ELSE()
    MESSAGE(STATUS "use libsagui from system")
    FIND_PATH(SG_INCLUDE_DIR "sagui.h" PATHS "/usr/include/sagui/" REQUIRED)
ENDIF()

# checking libevent exist or not. If not exist, compile from source
FIND_LIBRARY(event NAMES event)
IF("${event}" STREQUAL "event-NOTFOUND")
    MESSAGE(STATUS "no libevent, compile from source code")
    ExternalProject_Add(
        ${EV_PACKAGE_NAME}
        PREFIX ${CMAKE_BINARY_DIR}/demo/event
        GIT_REPOSITORY https://github.com/libevent/libevent.git
        GIT_TAG release-2.1.12-stable
        GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
        TIMEOUT 10
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
    )

    ExternalProject_Get_Property(${EV_PACKAGE_NAME} SOURCE_DIR)
    SET(EV_INCLUDE_DIR ${SOURCE_DIR}/include/ CACHE INTERNAL "Path to include folder for libevent package")
    ExternalProject_Get_Property(${EV_PACKAGE_NAME} BINARY_DIR)

    SET(EV_LIBRARY_NAME "${CMAKE_SHARED_MODULE_PREFIX}event${CMAKE_SHARED_LIBRARY_SUFFIX}")

    ExternalProject_Add_Step(
        ${EV_PACKAGE_NAME} copy2lib
        COMMAND ${CMAKE_COMMAND} -E tar cf event.tar lib
        COMMAND ${CMAKE_COMMAND} -E copy event.tar ${CMAKE_BINARY_DIR}/output
        WORKING_DIRECTORY ${BINARY_DIR}
        DEPENDEES install
    )

    ExternalProject_Add_Step(
        ${EV_PACKAGE_NAME} extract
        COMMAND ${CMAKE_COMMAND} -E tar xf event.tar
        COMMAND ${CMAKE_COMMAND} -E rm event.tar
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/output
        DEPENDEES copy2lib
    )
    ADD_LIBRARY(event SHARED IMPORTED)
    SET_PROPERTY(TARGET event PROPERTY IMPORTED_LOCATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${EV_LIBRARY_NAME})
ELSE()
    MESSAGE(STATUS "use libevent from system")
    FIND_PATH(EV_INCLUDE_DIR event.h REQUIRED)
ENDIF()


ADD_EXECUTABLE(${API_SERVER_NAME} ${srcs})

TARGET_LINK_LIBRARIES(${API_SERVER_NAME} PUBLIC ${PROJECT_NAME} sagui event)
TARGET_INCLUDE_DIRECTORIES(${API_SERVER_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/demo/inc ${JSON_INCLUDE_DIR} ${SG_INCLUDE_DIR} ${EV_INCLUDE_DIR})
