# Umicom Framework
# File: cmake/UmicomComfyInspectionTests.cmake
#
# PURPOSE:
#   Share console observation qualification for source and installed-SDK hosts.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
function(umicom_add_comfy_inspection_command_tests target prefix)
    cmake_parse_arguments(ARG "" "COMPARE_TARGET" "" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS OR NOT TARGET "${target}")
        message(FATAL_ERROR "Offline inspection tests require an existing host target")
    endif()
    set(driver_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/media_generation")
    if(NOT EXISTS "${driver_root}/check_comfy_inspection_command.cmake")
        set(driver_root "${UMICOM_FRAMEWORK_COMFY_TEST_ROOT}")
    endif()
    if(NOT EXISTS "${driver_root}/check_comfy_inspection_command.cmake")
        message(FATAL_ERROR "Selected Framework lacks the offline inspection qualification driver")
    endif()
    set(compare)
    if(ARG_COMPARE_TARGET)
        if(NOT TARGET "${ARG_COMPARE_TARGET}")
            message(FATAL_ERROR "Comparison target is unavailable")
        endif()
        list(APPEND compare "-DCOMPARE_EXE=$<TARGET_FILE:${ARG_COMPARE_TARGET}>")
    endif()
    foreach(mode complete missing malformed-document usage help unicode-path)
        add_test(NAME "${prefix}.${mode}" COMMAND "${CMAKE_COMMAND}"
            "-DHOST_EXE=$<TARGET_FILE:${target}>" ${compare}
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/${prefix}/${mode}"
            "-DTEST_CASE=${mode}" -P "${driver_root}/check_comfy_inspection_command.cmake")
        set_tests_properties("${prefix}.${mode}" PROPERTIES TIMEOUT 80 LABELS "media;comfy;offline;console")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${prefix}.${mode}" TARGETS "${target}")
        endif()
    endforeach()

    # The same host owns both routes. Reuse its file fixtures and optional peer
    # comparison; native execution remains separate from test registration.
    foreach(mode complete missing unknown-slot page last-page end beyond offset-overflow negative zero-limit limit-overflow duplicate unknown-option missing-value malformed-workflow relative help unicode-path options-order empty-client empty)
        add_test(NAME "${prefix}.references-${mode}" COMMAND "${CMAKE_COMMAND}"
            "-DHOST_EXE=$<TARGET_FILE:${target}>" ${compare}
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/${prefix}/references-${mode}"
            "-DTEST_CASE=${mode}" -P "${driver_root}/check_comfy_reference_command.cmake")
        set_tests_properties("${prefix}.references-${mode}" PROPERTIES TIMEOUT 80 LABELS "media;comfy;offline;console")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${prefix}.references-${mode}" TARGETS "${target}")
        endif()
    endforeach()
endfunction()


# Captured queue qualification is reusable on its own for small SDK consumers.
function(umicom_add_comfy_queue_command_tests target prefix)
    cmake_parse_arguments(ARG "" "COMPARE_TARGET" "" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS OR NOT TARGET "${target}" OR prefix STREQUAL "")
        message(FATAL_ERROR "Queue inspection tests require an existing host and a nonempty prefix")
    endif()
    set(driver_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/media_generation")
    if(NOT EXISTS "${driver_root}/check_comfy_queue_command.cmake")
        set(driver_root "${UMICOM_FRAMEWORK_COMFY_TEST_ROOT}")
    endif()
    if(NOT EXISTS "${driver_root}/check_comfy_queue_command.cmake")
        message(FATAL_ERROR "Selected Framework lacks the captured queue qualification driver")
    endif()
    set(compare)
    if(ARG_COMPARE_TARGET)
        if(NOT TARGET "${ARG_COMPARE_TARGET}")
            message(FATAL_ERROR "Queue comparison target is unavailable")
        endif()
        list(APPEND compare "-DCOMPARE_EXE=$<TARGET_FILE:${ARG_COMPARE_TARGET}>")
    endif()
    foreach(mode complete empty page last-page end beyond offset-overflow negative zero-limit limit-overflow duplicate unknown-option missing-value malformed-queue disabled malformed-document relative help unicode-path options-order missing-file directory extra queue-limit)
        add_test(NAME "${prefix}.${mode}" COMMAND "${CMAKE_COMMAND}"
            "-DHOST_EXE=$<TARGET_FILE:${target}>" ${compare}
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/${prefix}/${mode}"
            "-DTEST_CASE=${mode}" -P "${driver_root}/check_comfy_queue_command.cmake")
        set_tests_properties("${prefix}.${mode}" PROPERTIES TIMEOUT 80 LABELS "media;comfy;queue;offline;console")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${prefix}.${mode}" TARGETS "${target}")
        endif()
    endforeach()
endfunction()


# System reports use the same command contract in every host, including a small
# installed-SDK consumer. The driver owns only its separate binary-directory fixtures.
function(umicom_add_comfy_system_command_tests target prefix)
    cmake_parse_arguments(ARG "" "COMPARE_TARGET" "" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS OR NOT TARGET "${target}" OR prefix STREQUAL "")
        message(FATAL_ERROR "System observation tests require an existing host and a nonempty prefix")
    endif()
    set(driver_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/media_generation")
    if(NOT EXISTS "${driver_root}/check_comfy_system_command.cmake")
        set(driver_root "${UMICOM_FRAMEWORK_COMFY_TEST_ROOT}")
    endif()
    if(NOT EXISTS "${driver_root}/check_comfy_system_command.cmake")
        message(FATAL_ERROR "Selected Framework lacks the captured system qualification driver")
    endif()
    set(compare)
    if(ARG_COMPARE_TARGET)
        if(NOT TARGET "${ARG_COMPARE_TARGET}")
            message(FATAL_ERROR "System comparison target is unavailable")
        endif()
        list(APPEND compare "-DCOMPARE_EXE=$<TARGET_FILE:${ARG_COMPARE_TARGET}>")
    endif()
    foreach(mode complete empty unicode-path disabled malformed-document malformed-system missing-file directory system-limit document-limit empty-file relative help usage extra)
        add_test(NAME "${prefix}.${mode}" COMMAND "${CMAKE_COMMAND}"
            "-DHOST_EXE=$<TARGET_FILE:${target}>" ${compare}
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/${prefix}/${mode}"
            "-DTEST_CASE=${mode}" -P "${driver_root}/check_comfy_system_command.cmake")
        set_tests_properties("${prefix}.${mode}" PROPERTIES TIMEOUT 80 LABELS "media;comfy;system;offline;console")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${prefix}.${mode}" TARGETS "${target}")
        endif()
    endforeach()
endfunction()
