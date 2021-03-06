cmake_minimum_required(VERSION 3.4)
project(arango-node-driver)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)

set(VELOCYPACK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/velocypack)

option(BuildVelocyPackExamples "Build examples" OFF)
option(BuildTools "Build support programs and tools" OFF)

add_subdirectory(dependencies/fuerte)

add_library(arango-node-driver SHARED
    src/node_init.cpp
    src/node_vpack.cpp
    src/node_request.cpp
    src/node_response.cpp
    src/node_connection.cpp
    src/node_connection_builder.cpp
)

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(arango-node-driver PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
target_include_directories(arango-node-driver PRIVATE ${CMAKE_JS_INC})

# Essential library files to link to a node addon
# You should add this line in every CMake.js based project
target_link_libraries(arango-node-driver ${CMAKE_JS_LIB} fuerte)

#########################################################################################
# Clang Completion
#copy compile commands to source dir
if( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
    message(STATUS "copy compile_commands.json")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()
