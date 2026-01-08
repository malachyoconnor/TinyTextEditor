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
        Editor.cpp
        Escape.h
        SpecialKey.h)

add_executable(KeyPressDetector
        KeyPressDetector.cpp EditorState.cpp Editor.h Terminal.h Terminal.cpp utils.h)

# Finds me the root directory of the project - useful for testing
target_compile_definitions(TinyTextEditor PRIVATE ROOT_DIRECTORY="${PROJECT_SOURCE_DIR}")

# Makes the binaries output to the root directory
set_target_properties(TinyTextEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)
set_target_properties(KeyPressDetector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)