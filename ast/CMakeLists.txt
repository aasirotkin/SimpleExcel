cmake_minimum_required(VERSION 3.0.0)

project(ast VERSION 0.0.1 LANGUAGES CXX)

set(ANTLR_DIR CACHE STRING "Antlr static library directory")
if(NOT ANTLR_DIR)
    message(FATAL_ERROR "Please, specify ANTLR_DIR directory via -DANTLR_DIR=<dir>")
endif()
message(STATUS "Antlr dir is ${ANTLR_DIR}, chane via -DANTLR_DIR=<dir>")

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(./ SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})

include_directories(${PROJECT_NAME} "${ANTLR_DIR}/src")
link_directories(${ANTLR_LIBS} "$<IF:$<CONFIG:Debug>,"${ANTLR_DIR}/lib/debug","${ANTLR_DIR}/lib/release">")
target_link_libraries(${PROJECT_NAME} -lantlr4-runtime)

set(CMAKE_CXX_FLAGS "-Wall")

if (NOT CMAKE_SYSTEM_NAME MATCHES ".*Win.*")
    target_link_libraries(${PROJECT_NAME} -ltbb -lpthread)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wpedantic")
endif()

message(STATUS "System name is ${CMAKE_SYSTEM_NAME}")
if (NOT CMAKE_BUILD_CONFIG)
    message(STATUS "CMake build config is not specified")
else()
    message(STATUS "CMake build config is ${CMAKE_BUILD_CONFIG}")
endif()

# add_definitions()

