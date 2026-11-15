#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardCancellation.cmake
#
# PURPOSE:
# Register cooperative scanner cancellation and legacy behavior checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    add_executable(umicom-codeguard-cancellation-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_scan_cancellation.c")
    target_link_libraries(umicom-codeguard-cancellation-tests PRIVATE Umicom::codeguard)
    target_compile_features(umicom-codeguard-cancellation-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-codeguard-cancellation-tests)
    umicom_apply_sanitizers(umicom-codeguard-cancellation-tests)
    foreach(_case IN ITEMS arguments preflight unsupported missing null-token legacy file-legacy
        walk walk-error registry read read-error pattern pattern-error lifetime-error architecture duplicate-add duplicate-emit duplicate-add-error duplicate-emit-error
        complete empty)
        set(_work "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-cancellation/${_case}")
        file(MAKE_DIRECTORY "${_work}")
        add_test(NAME "framework.codeguard_cancellation.${_case}"
            # Retain the original absolute-root invocation for review:
            # COMMAND umicom-codeguard-cancellation-tests "${_case}" "${_work}")
            COMMAND umicom-codeguard-cancellation-tests "${_case}" ".")
        set_tests_properties("framework.codeguard_cancellation.${_case}" PROPERTIES
            # Relative scan roots isolate scanner boundary tests from the
            # existing absolute-path build-directory exclusion policy.
            WORKING_DIRECTORY "${_work}"
            TIMEOUT 20 LABELS "codeguard;cancellation;filesystem")
    endforeach()
endif()
