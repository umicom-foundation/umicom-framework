#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/bounded_native_services/CMakeLists.txt
#
# PURPOSE:
#   Build an independent consumer of the exported native service libraries.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)
project(UmicomBoundedServices LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
include(CTest)
# Consume exported SDK targets; no source checkout or network fetch is needed.
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS game_engine render3d media_engine web)
add_executable(bounded-services main.c)
target_link_libraries(bounded-services PRIVATE Umicom::game_engine
    Umicom::render3d Umicom::media_engine Umicom::web)
if(BUILD_TESTING)
    add_test(NAME bounded-services.result COMMAND bounded-services)
    set_tests_properties(bounded-services.result PROPERTIES TIMEOUT 10)
endif()
