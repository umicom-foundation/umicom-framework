# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStoryboardBridge.cmake
# PURPOSE: Compose a bounded JSON binding over the canonical creative workspace.
# EDUCATIONAL OVERVIEW: AUTO reports unavailable when the SDK target is absent;
# ON requires real linkage. It never inserts a mock implementation into production.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
include_guard(GLOBAL)
set(UMICOM_WORKER_STORYBOARD "AUTO" CACHE STRING "Canonical storyboard bridge: AUTO, ON or OFF")
set_property(CACHE UMICOM_WORKER_STORYBOARD PROPERTY STRINGS AUTO ON OFF)
if(NOT UMICOM_WORKER_STORYBOARD MATCHES "^(AUTO|ON|OFF)$")
    message(FATAL_ERROR "UMICOM_WORKER_STORYBOARD must be AUTO, ON or OFF")
endif()
if(UMICOM_WORKER_STORYBOARD STREQUAL "ON" AND NOT TARGET Umicom::creative_workspace)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()
if(UMICOM_WORKER_STORYBOARD STREQUAL "ON" AND NOT TARGET Umicom::creative_workspace)
    message(FATAL_ERROR "The canonical Umicom::creative_workspace SDK target is required")
endif()
add_library(umicom_ai_storyboard_bridge STATIC "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/storyboard_bridge.c")
add_library(Umicom::ai_storyboard_bridge ALIAS umicom_ai_storyboard_bridge)
set_target_properties(umicom_ai_storyboard_bridge PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_link_libraries(umicom_ai_storyboard_bridge PUBLIC Umicom::ai_worker_transport)
target_link_libraries(umicom_ai_storyboard_bridge PRIVATE PkgConfig::WORKER_JSONC)
if(TARGET Umicom::creative_workspace AND NOT UMICOM_WORKER_STORYBOARD STREQUAL "OFF")
    target_link_libraries(umicom_ai_storyboard_bridge PRIVATE Umicom::creative_workspace)
    target_compile_definitions(umicom_ai_storyboard_bridge PRIVATE UMICOM_STORYBOARD_LINKED=1)
    message(STATUS "Canonical storyboard bridge: linked")
else()
    message(STATUS "Canonical storyboard bridge: unavailable (drafting remains available)")
endif()
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom_ai_storyboard_bridge PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
