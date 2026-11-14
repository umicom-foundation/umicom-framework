# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPeImports.cmake
# PURPOSE: Compose bounded PE import inspection with the canonical binary inspector.
# EDUCATIONAL OVERVIEW: Existing SDK targets are reused when present; a focused
# source build compiles the same canonical file rather than a copied substitute.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umi_pe_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::pe_imports)
    add_library(umicom_pe_imports STATIC "${_umi_pe_root}/src/education/pe_imports.c")
    add_library(Umicom::pe_imports ALIAS umicom_pe_imports)
    set_target_properties(umicom_pe_imports PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_include_directories(umicom_pe_imports PUBLIC "${_umi_pe_root}/include")
    if(TARGET Umicom::education_inspection)
        target_link_libraries(umicom_pe_imports PUBLIC Umicom::education_inspection)
    else()
        target_sources(umicom_pe_imports PRIVATE "${_umi_pe_root}/src/education/binary_inspect.c")
    endif()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom_pe_imports PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
