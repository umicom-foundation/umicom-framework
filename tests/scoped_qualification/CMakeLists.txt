# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Compile the actual ledger/archive sources with a bounded regression selection.
cmake_minimum_required(VERSION 3.24)
project(UmicomScopedQualification LANGUAGES C)
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(FATAL_ERROR "Configure this focused project independently; Framework owns the normal targets.")
endif()
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
enable_testing()
get_filename_component(_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

# These are test-only subjects, not replacement SDK exports or installed copies.
# Compile production paths directly so the focused checks cannot drift into a
# second implementation. Full Framework remains responsible for composition,
# optional database/GTK adapters, actual process execution and SDK installation.
add_library(umi-qualification-core STATIC
    "${_root}/src/base/status.c"
    "${_root}/src/base/sha256.c"
    "${_root}/src/platform/input_file.c"
    "${_root}/src/platform/output_file.c"
    "${_root}/src/distribution/runtime/inventory.c"
    "${_root}/src/distribution/runtime/inventory_text.c"
    "${_root}/src/distribution/runtime/inventory_identity.c"
    "${_root}/src/distribution/runtime/impact.c"
    "${_root}/src/distribution/runtime/impact_path.c"
    "${_root}/src/distribution/runtime/impact_metadata.c"
    "${_root}/src/distribution/runtime/impact_expression.c"
    "${_root}/src/distribution/runtime/source_observation.c"
    "${_root}/src/distribution/runtime/source_observation_paths.c"
    "${_root}/src/distribution/runtime/qualification.c"
    "${_root}/src/distribution/runtime/qualification_identity.c"
    "${_root}/src/distribution/runtime/qualification_assessment.c"
    "${_root}/src/distribution/runtime/qualification_report.c"
    "${_root}/src/distribution/runtime/qualification_batch.c"
    "${_root}/src/distribution/runtime/qualification_view.c"
    "${_root}/src/distribution/runtime/qualification_compare.c"
    "${_root}/src/distribution/runtime/qualification_parse_fields.c"
    "${_root}/src/distribution/runtime/qualification_parse.c"
    "${_root}/src/distribution/runtime/qualification_archive.c")
target_include_directories(umi-qualification-core PUBLIC "${_root}/include")
add_library(Umicom::distribution ALIAS umi-qualification-core)
function(umicom_apply_warnings target)
    if(MSVC)
        target_compile_options("${target}" PRIVATE /W4)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options("${target}" PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()
umicom_apply_warnings(umi-qualification-core)
# Use the normal registrations and fixtures rather than a smaller imitation.
include("${_root}/tests/release_qualification/records.cmake")
