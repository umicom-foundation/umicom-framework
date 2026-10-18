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


set(UMICOM_TEST_EXTERNAL_WORK_ROOT "" CACHE PATH
    "Optional non-repository fixture root for tests that exercise Git boundaries")

# Walk physical ancestors, including .git files used by worktrees/submodules.
# Return the directory outside the outermost checkout, or an empty value when
# the supplied location already lies outside all detected repositories.
function(umicom_test_repository_outer_parent directory_path output)
    file(REAL_PATH "${directory_path}" _cursor)
    set(_outside "")
    while(TRUE)
        get_filename_component(_parent "${_cursor}" DIRECTORY)
        if(EXISTS "${_cursor}/.git")
            if(_parent STREQUAL _cursor OR _parent STREQUAL "")
                message(FATAL_ERROR "Cannot place a non-repository test fixture outside ${_cursor}")
            endif()
            set(_outside "${_parent}")
        endif()
        if(_parent STREQUAL _cursor OR _parent STREQUAL "")
            break()
        endif()
        set(_cursor "${_parent}")
    endwhile()
    set("${output}" "${_outside}" PARENT_SCOPE)
endfunction()

# A report privacy test must exercise a real non-repository location. Redirect
# only tests declaring this requirement; do not weaken production path policy.
function(umicom_test_external_workspace requested_root output)
    if(NOT UMICOM_TEST_EXTERNAL_WORK_ROOT STREQUAL "")
        if(UMICOM_TEST_EXTERNAL_WORK_ROOT MATCHES "[;\r\n]" OR
           UMICOM_TEST_EXTERNAL_WORK_ROOT MATCHES "\\$<")
            message(FATAL_ERROR "UMICOM_TEST_EXTERNAL_WORK_ROOT cannot contain list separators or generator expressions")
        endif()
        get_filename_component(_root "${UMICOM_TEST_EXTERNAL_WORK_ROOT}" ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
        umicom_test_repository_outer_parent("${_root}" _outside)
        if(NOT _outside STREQUAL "")
            message(FATAL_ERROR "UMICOM_TEST_EXTERNAL_WORK_ROOT must be outside Git checkouts: ${_root}")
        endif()
    else()
        set(_root "${requested_root}")
        umicom_test_repository_outer_parent("${_root}" _outside)
        if(NOT _outside STREQUAL "")
            set(_root "${_outside}/_local/test-work")
        endif()
    endif()
    file(MAKE_DIRECTORY "${_root}")
    file(REAL_PATH "${_root}" _root)
    umicom_test_repository_outer_parent("${_root}" _outside)
    if(NOT _outside STREQUAL "")
        message(FATAL_ERROR "The external test root resolves inside a Git checkout: ${_root}")
    endif()
    set("${output}" "${_root}" PARENT_SCOPE)
endfunction()

# A nested configure creates its own compiler probes, project and build folders.
# Keep its leaf compact while retaining the build/directory/test identity digest.
# Ordinary test names remain readable and keep their previous locations.
function(umicom_test_workspace_leaf test_name test_directory compact output)
    string(REGEX REPLACE "[^A-Za-z0-9_-]" "-" _label "${test_name}")
    string(SUBSTRING "${_label}" 0 40 _label)
    if(compact)
        set(_label "build")
    endif()
    string(SHA256 _digest "${CMAKE_BINARY_DIR}|${test_directory}|${test_name}")
    string(SUBSTRING "${_digest}" 0 16 _digest)
    set("${output}" "${_label}-${_digest}" PARENT_SCOPE)
endfunction()

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
    get_property(_outside_required TEST "${test_name}" ${_scope} PROPERTY UMICOM_TEST_OUTSIDE_REPOSITORY)
    # The original Git-boundary-only selection is retained for review.
    #[[
    if(_outside_required)
        umicom_test_external_workspace("${_root}" _root)
    endif()
    ]]
    # Nested real-toolchain fixtures also need an external root: a short leaf
    # alone cannot recover the path budget consumed by a deeply nested build tree.
    get_property(_nested_build TEST "${test_name}" ${_scope} PROPERTY UMICOM_TEST_NESTED_BUILD)
    if(_outside_required OR _nested_build)
        umicom_test_external_workspace("${_root}" _root)
    endif()
    # Names remain recognisable. The digest distinguishes punctuation variants,
    # repeated test names in separate directories and separate build trees.
    # Shared leaf generation preserves ordinary names and shortens only nested
    # builds. Retain the original calculation so its identity is reviewable.
    #[[
    string(REGEX REPLACE "[^A-Za-z0-9_-]" "-" _label "${test_name}")
    string(SUBSTRING "${_label}" 0 40 _label)
    string(SHA256 _digest "${CMAKE_BINARY_DIR}|${test_directory}|${test_name}")
    string(SUBSTRING "${_digest}" 0 16 _digest)
    set(_directory "${_root}/${_label}-${_digest}")
    ]]
    umicom_test_workspace_leaf("${test_name}" "${test_directory}" "${_nested_build}" _leaf)
    set(_directory "${_root}/${_leaf}")
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

# These checks need an actual non-repository environment before constructing
# their synthetic .git markers; they never inspect or modify real Git metadata.
if(BUILD_TESTING AND NOT CMAKE_SCRIPT_MODE_FILE)
    foreach(_fixture_case plain git-directory git-file nested explicit-external explicit-inside)
        add_test(NAME "framework.test_workspace.external.${_fixture_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DUMICOM_FRAMEWORK_SOURCE=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DCASE=${_fixture_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/test_runtime/check_external_fixture_root.cmake")
        set_tests_properties("framework.test_workspace.external.${_fixture_case}" PROPERTIES
            UMICOM_TEST_OUTSIDE_REPOSITORY TRUE TIMEOUT 30)
    endforeach()
endif()

# Pure path-contract checks run without a compiler or a generated application.
if(BUILD_TESTING AND NOT CMAKE_SCRIPT_MODE_FILE)
    foreach(_leaf_case ordinary compact distinct-name distinct-directory distinct-build)
        add_test(NAME "framework.test_workspace.leaf.${_leaf_case}"
            COMMAND "${CMAKE_COMMAND}"
                "-DUMICOM_FRAMEWORK_SOURCE=${CMAKE_CURRENT_LIST_DIR}/.."
                "-DCASE=${_leaf_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/test_runtime/check_fixture_leaf.cmake")
        set_tests_properties("framework.test_workspace.leaf.${_leaf_case}" PROPERTIES
            TIMEOUT 15 LABELS "framework;test-workspace;regression")
    endforeach()
endif()
