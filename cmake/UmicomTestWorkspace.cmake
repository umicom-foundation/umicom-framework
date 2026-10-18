#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTestWorkspace.cmake
# PURPOSE: Keep test fixtures in an explicit, accessible, isolated location.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(UMICOM_TEST_WORK_ROOT "${CMAKE_BINARY_DIR}/test-work" CACHE PATH
    "Root for retained test fixtures; defaults to this build's test-work folder")

function(umicom_configure_test_workspace test_name test_directory)
    # Cross-directory TEST properties require CMake 3.28. Preserve support for
    # older standalone projects when the test belongs to the current directory.
    if(CMAKE_VERSION VERSION_LESS "3.28" AND NOT test_directory STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        return()
    endif()
    set(_scope)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
        set(_scope DIRECTORY "${test_directory}")
    endif()
    get_property(_configured TEST "${test_name}" ${_scope} PROPERTY UMICOM_TEST_WORK_CONFIGURED)
    if(_configured)
        return()
    endif()
    get_property(_environment TEST "${test_name}" ${_scope} PROPERTY ENVIRONMENT)
    get_property(_modifications TEST "${test_name}" ${_scope} PROPERTY ENVIRONMENT_MODIFICATION)
    # An explicit fixture override owns the complete temp policy. Do not add
    # TMPDIR alongside its TEMP setting: GLib would prefer the new TMPDIR.
    foreach(_entry IN LISTS _environment _modifications)
        if(_entry MATCHES "^(TMPDIR|TMP|TEMP)=")
            set_property(TEST "${test_name}" ${_scope} PROPERTY UMICOM_TEST_WORK_CONFIGURED TRUE)
            return()
        endif()
    endforeach()
    if(UMICOM_TEST_WORK_ROOT STREQUAL "" OR UMICOM_TEST_WORK_ROOT MATCHES "[;\r\n]" OR
       UMICOM_TEST_WORK_ROOT MATCHES "\\$<")
        message(FATAL_ERROR "UMICOM_TEST_WORK_ROOT must be a nonempty path without list separators or generator expressions")
    endif()
    get_filename_component(_root "${UMICOM_TEST_WORK_ROOT}" ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
    file(TO_CMAKE_PATH "${_root}" _root)
    # Names remain recognisable. The digest distinguishes punctuation variants,
    # repeated test names in separate directories and separate build trees.
    string(REGEX REPLACE "[^A-Za-z0-9_-]" "-" _label "${test_name}")
    string(SUBSTRING "${_label}" 0 40 _label)
    string(SHA256 _digest "${CMAKE_BINARY_DIR}|${test_directory}|${test_name}")
    string(SUBSTRING "${_digest}" 0 16 _digest)
    set(_directory "${_root}/${_label}-${_digest}")
    file(MAKE_DIRECTORY "${_directory}")
    set_property(TEST "${test_name}" ${_scope} APPEND PROPERTY ENVIRONMENT
        "TMPDIR=${_directory}" "TEMP=${_directory}" "TMP=${_directory}"
        "UMICOM_TEST_WORK_DIRECTORY=${_directory}")
    set_property(TEST "${test_name}" ${_scope} PROPERTY UMICOM_TEST_WORK_CONFIGURED TRUE)
endfunction()

function(umicom_configure_test_workspaces directory_path)
    get_property(_tests DIRECTORY "${directory_path}" PROPERTY TESTS)
    foreach(_test IN LISTS _tests)
        umicom_configure_test_workspace("${_test}" "${directory_path}")
    endforeach()
    get_property(_children DIRECTORY "${directory_path}" PROPERTY SUBDIRECTORIES)
    foreach(_child IN LISTS _children)
        # Binary directories identify the configured instance of a reused source.
        get_property(_binary DIRECTORY "${_child}" PROPERTY BINARY_DIR)
        umicom_configure_test_workspaces("${_binary}")
    endforeach()
endfunction()
