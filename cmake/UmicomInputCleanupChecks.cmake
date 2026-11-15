#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomInputCleanupChecks.cmake
#
# PURPOSE:
# Register bounded input cleanup and first-failure status checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    add_executable(umicom-input-cleanup-status-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/test_input_cleanup_status.c")
    target_link_libraries(umicom-input-cleanup-status-tests PRIVATE Umicom::platform)
    target_compile_features(umicom-input-cleanup-status-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-input-cleanup-status-tests)
    umicom_apply_sanitizers(umicom-input-cleanup-status-tests)
    set(_input_root "${CMAKE_CURRENT_BINARY_DIR}/tests/input-cleanup")
    file(MAKE_DIRECTORY "${_input_root}")
    file(WRITE "${_input_root}/bytes.bin" "0123456789ABCDEF")
    file(WRITE "${_input_root}/empty.bin" "")
    foreach(_case IN ITEMS file-success file-close file-capacity file-capacity-close
        file-allocation file-allocation-close file-changed file-changed-close
        file-read file-read-close file-empty file-empty-close stream-success
        stream-close stream-capacity stream-capacity-close stream-changed
        stream-changed-close stream-read stream-read-close stream-cancel
        stream-cancel-close stream-callback-oom-close stream-callback-capacity-close
        stream-callback-invalid-close stream-empty stream-empty-close)
        add_test(NAME "framework.input_cleanup_status.${_case}"
            COMMAND umicom-input-cleanup-status-tests "${_case}"
                "${_input_root}/bytes.bin" "${_input_root}/empty.bin")
        set_tests_properties("framework.input_cleanup_status.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "platform;input;cleanup;fault-injection")
    endforeach()
endif()
