#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build_configuration/check_native_branding.cmake
#
# PURPOSE:
#   Build a native branding fixture and compare staged artwork with canonical resources.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
foreach(_required IN ITEMS ROOT DEST COMPILER)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing native branding fixture argument: ${_required}")
    endif()
endforeach()
get_filename_component(ROOT "${ROOT}" ABSOLUTE)
find_program(_ninja NAMES ninja ninja-build REQUIRED)
file(MAKE_DIRECTORY "${DEST}")
function(native_brand_step name)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE _result
        OUTPUT_FILE "${DEST}/${name}.log" ERROR_FILE "${DEST}/${name}-errors.log")
    if(NOT _result STREQUAL "0")
        message(FATAL_ERROR "Native branding ${name} failed (${_result}); inspect ${DEST}/${name}-errors.log")
    endif()
endfunction()
native_brand_step(configure "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/build_configuration/native_branding" -B "${DEST}/build"
    -G Ninja "-DCMAKE_MAKE_PROGRAM=${_ninja}" "-DCMAKE_C_COMPILER=${COMPILER}"
    "-DFRAMEWORK_ROOT=${ROOT}")
native_brand_step(build "${CMAKE_COMMAND}" --build "${DEST}/build" --target native-branding)
foreach(_file IN ITEMS umicom-icon.svg umicom-icon-on-dark.svg umicom-logo.svg umicom-logo-on-dark.svg)
    set(_published "${DEST}/build/bin/branding/${_file}")
    if(NOT EXISTS "${_published}")
        message(FATAL_ERROR "Missing staged brand resource: ${_file}")
    endif()
    file(SHA256 "${ROOT}/resources/brand/${_file}" _expected)
    file(SHA256 "${_published}" _actual)
    if(NOT _actual STREQUAL _expected)
        message(FATAL_ERROR "Staged brand bytes differ: ${_file}")
    endif()
endforeach()
# Compilation and resource identity do not establish rendering or bootability.
