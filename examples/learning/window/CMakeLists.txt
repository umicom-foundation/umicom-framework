#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning/window/CMakeLists.txt
# PURPOSE: Build the window exercise against an installed Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomLearningWindow LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
add_executable(umicom-learning-window main.c)
set_target_properties(umicom-learning-window PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_link_libraries(umicom-learning-window PRIVATE Umicom::ui_gtk4)
if(MSVC)
    target_compile_options(umicom-learning-window PRIVATE /W4)
else()
    target_compile_options(umicom-learning-window PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
install(TARGETS umicom-learning-window RUNTIME DESTINATION bin)
