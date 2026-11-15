#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardReview.cmake
#
# PURPOSE:
# Register current-request quality review and lifetime regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    add_executable(umicom-codeguard-review-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_review_controller.c")
    target_link_libraries(umicom-codeguard-review-tests PRIVATE Umicom::codeguard)
    target_compile_features(umicom-codeguard-review-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-codeguard-review-tests)
    umicom_apply_sanitizers(umicom-codeguard-review-tests)
    set(_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-review")
    file(MAKE_DIRECTORY "${_root}/workspace")
    file(CONFIGURE OUTPUT "${_root}/workspace/sample.c"
        CONTENT "void sample(void) {\n    malloc(4);\n}\n" NEWLINE_STYLE LF)
    foreach(_case IN ITEMS arguments idle success empty budget missing cancel-terminal cancel-failure
        cancel-after-failure stale-terminal stale-cancel-failure new-context wrong-owner busy queued-cancel queue-stopped
        allocation service-allocation job-allocation submit-failure read-failure retirement-failure
        request-overflow profile-copy destroy-terminal)
        add_test(NAME "framework.codeguard_review.${_case}"
            COMMAND umicom-codeguard-review-tests "${_case}" "${_root}")
        set_tests_properties("framework.codeguard_review.${_case}" PROPERTIES
            TIMEOUT 25 LABELS "codeguard;review;jobs;ownership")
    endforeach()
endif()
