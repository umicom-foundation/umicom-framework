#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/image_test_composition/check.cmake
#
# PURPOSE:
#   Configure and reconfigure an isolated consumer graph, retaining the actual diagnostic on failure.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
set(_configure "${CMAKE_COMMAND}" -S "${FIXTURE}" -B "${WORK}"
    -G "${GENERATOR}" "-DFRAMEWORK_ROOT=${FRAMEWORK_ROOT}"
    "-DCASE=${CASE}" "-DCMAKE_C_COMPILER=${C_COMPILER}")
if(MAKE_PROGRAM)
    list(APPEND _configure "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}")
endif()
if(GENERATOR_PLATFORM)
    list(APPEND _configure -A "${GENERATOR_PLATFORM}")
endif()
if(GENERATOR_TOOLSET)
    list(APPEND _configure -T "${GENERATOR_TOOLSET}")
endif()
file(MAKE_DIRECTORY "${WORK}")
# A registration guard is scoped to one configure process, not stored in the
# cache. Reconfiguration must recreate the complete test graph on every run.
foreach(_pass IN ITEMS initial repeated)
    execute_process(COMMAND ${_configure} RESULT_VARIABLE _result
        OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 50)
    file(WRITE "${WORK}/${_pass}.log" "${_out}\n${_err}")
    if(NOT "${_result}" STREQUAL "0")
        message(FATAL_ERROR "Image test composition ${CASE}/${_pass} failed (${_result}):\n${_out}\n${_err}")
    endif()
endforeach()
