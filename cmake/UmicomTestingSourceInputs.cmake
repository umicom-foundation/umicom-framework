# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTestingSourceInputs.cmake
# PURPOSE: Reject incomplete testing sources during configuration.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
include_guard(GLOBAL)

# These fragments are compiled through ctest_adapter.c. Listing them here
# makes source delivery failures visible during configure, before the build
# has compiled thousands of unrelated files. Extend the list when splitting
# another part of the adapter into a private fragment.
set(_umicom_testing_inputs
    src/testing/ctest_report.inc
    src/testing/ctest_report_files.inc
    src/testing/ctest_job.inc
    include/umicom/testing/ctest_job.h)
foreach(_input IN LISTS _umicom_testing_inputs)
    set(_absolute "${CMAKE_CURRENT_LIST_DIR}/../${_input}")
    if(NOT EXISTS "${_absolute}" OR IS_DIRECTORY "${_absolute}")
        message(FATAL_ERROR
            "Framework testing source is missing: ${_input}. Restore the complete matching Framework source, including .inc files, then configure again.")
    endif()
    set_source_files_properties("${_absolute}" PROPERTIES HEADER_FILE_ONLY TRUE)
    target_sources(umicom_testing PRIVATE "${_absolute}")
endforeach()
