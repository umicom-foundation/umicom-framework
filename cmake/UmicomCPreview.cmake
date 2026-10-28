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
    find_package(UmicomC 0.1 CONFIG REQUIRED)
    get_filename_component(_framework "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    # An explicit optional SDK adapter avoids making the compiler bootstrap
    # depend on the complete Framework or its desktop dependencies.
    add_library("${target}" STATIC "${_framework}/src/toolchain/cvm_preview.c")
    target_include_directories("${target}" PUBLIC "${_framework}/include")
    target_compile_features("${target}" PUBLIC c_std_23)
    target_link_libraries("${target}" PUBLIC UmicomC::Compiler)
    set_target_properties("${target}" PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options("${target}" PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endfunction()
