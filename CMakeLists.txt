cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(simple_excel VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  set(
    CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} /JMC"
  )
else()
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough"
  )
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/FindANTLR.cmake)

add_definitions(
  -DANTLR4CPP_STATIC
  -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)

add_subdirectory(antlr4_runtime)

include_directories(
  ${ANTLR4_INCLUDE_DIRS}
  ${ANTLR_FormulaParser_OUTPUT_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/antlr4_runtime/runtime/src
)

aux_source_directory(src/ SRC_LIST)
aux_source_directory(antlr4_formula/ FORMULA_SRC_LIST)

add_executable(
  ${PROJECT_NAME}
  ${ANTLR_FormulaParser_CXX_OUTPUTS}
  ${SRC_LIST}
  ${FORMULA_SRC_LIST}
  )

target_link_libraries(${PROJECT_NAME} antlr4_static)
if(MSVC)
  target_compile_options(antlr4_static PRIVATE /W0)
endif()

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION bin
  EXPORT ${PROJECT_NAME}
)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})
