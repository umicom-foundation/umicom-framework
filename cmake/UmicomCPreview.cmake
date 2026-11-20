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
function(umicom_require_cvm_sdk)
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
endfunction()

function(umicom_add_cvm_preview target)
    if(TARGET "${target}")
        message(FATAL_ERROR "CVM preview target already exists: ${target}")
    endif()
    # Preview and translation share the same explicitly selected SDK.
    umicom_require_cvm_sdk()
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

# Source warnings are an explicit newer-SDK composition. Existing preview
# consumers keep their previous dependency contract until they opt in.
function(umicom_add_cvm_source_check target)
    umicom_add_cvm_preview("${target}")
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    target_sources("${target}" PRIVATE "${_framework}/src/toolchain/cvm_source_check.c")
endfunction()

# Document-aware clients explicitly supply their platform owner. Keeping this
# optional avoids pulling editor and filesystem services into a compiler-only
# application or changing existing preview consumers' dependency contracts.
function(umicom_add_cvm_document_check target platform_target)
    if(NOT TARGET "${platform_target}")
        message(FATAL_ERROR "Document checks require an existing Framework platform target")
    endif()
    umicom_add_cvm_source_check("${target}")
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    target_sources("${target}" PRIVATE "${_framework}/src/toolchain/cvm_document_check.c")
    target_link_libraries("${target}" PUBLIC "${platform_target}")
endfunction()

function(umicom_add_cvm_file_translation target platform_target)
    if(NOT TARGET "${platform_target}")
        message(FATAL_ERROR "File translation requires an existing Framework platform target")
    endif()
    umicom_require_cvm_sdk()
    if(NOT TARGET UmicomC::Translation)
        message(FATAL_ERROR "Select an Umicom C SDK that exports UmicomC::Translation for file translation")
    endif()
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    add_library("${target}" STATIC "${_framework}/src/toolchain/cvm_file_translation.c")
    target_include_directories("${target}" PUBLIC "${_framework}/include")
    target_compile_features("${target}" PUBLIC c_std_23)
    target_link_libraries("${target}" PUBLIC "${platform_target}" UmicomC::Translation)
    set_target_properties("${target}" PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options("${target}" PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endfunction()
