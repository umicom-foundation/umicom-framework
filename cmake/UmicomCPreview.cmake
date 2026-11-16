#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCPreview.cmake
#
# PURPOSE:
#   Expose bounded source preview with explicit result ownership.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------
include_guard(GLOBAL)
function(umicom_add_cvm_preview target)
    if(TARGET "${target}")
        message(FATAL_ERROR "CVM preview target already exists: ${target}")
    endif()
    # The former unconditional package lookup is retained for review. The
    # installed SDK remains the default in the else branch below.
    # find_package(UmicomC 0.1 CONFIG REQUIRED)
    # Function-local option values avoid rewriting the caller's cache.
    # An explicit source checkout supports configuration review before an SDK
    # install. The default remains the installed package; never select a sibling
    # checkout implicitly or build the full Framework to bootstrap the compiler.
    if(UMICOM_C_PREVIEW_SOURCE)
        if(NOT IS_ABSOLUTE "${UMICOM_C_PREVIEW_SOURCE}" OR NOT EXISTS "${UMICOM_C_PREVIEW_SOURCE}/include/umicom_c/machine.h")
            message(FATAL_ERROR "UMICOM_C_PREVIEW_SOURCE must select the canonical resumable compiler source")
        endif()
        if(NOT TARGET UmicomC::Compiler)
            set(UMICOM_C_BUILD_TOOLS OFF)
            set(UMICOM_C_BUILD_TESTS OFF)
            add_subdirectory("${UMICOM_C_PREVIEW_SOURCE}" "${CMAKE_CURRENT_BINARY_DIR}/umicom-c")
        endif()
    else()
        find_package(UmicomC 0.1 CONFIG REQUIRED)
    endif()
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    # An explicit optional SDK adapter avoids making the compiler bootstrap
    # depend on the complete Framework or its desktop dependencies.
    add_library("${target}" STATIC "${_framework}/src/toolchain/cvm_preview.c")
    target_sources("${target}" PRIVATE "${_framework}/src/toolchain/cvm_debug.c")
    target_include_directories("${target}" PUBLIC "${_framework}/include")
    target_compile_features("${target}" PUBLIC c_std_23)
    target_link_libraries("${target}" PUBLIC UmicomC::Compiler)
    set_target_properties("${target}" PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options("${target}" PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endfunction()
