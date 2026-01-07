cmake_minimum_required(VERSION 4.1)
project(TinyTextEditor)

set(CMAKE_CXX_STANDARD 23)

add_executable(TinyTextEditor main.cpp
        AppendBuffer.h
        utils.h
        EditorState.h
        Terminal.h
        Terminal.cpp
        EditorState.cpp
        Editor.cpp)
set_target_properties(TinyTextEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)