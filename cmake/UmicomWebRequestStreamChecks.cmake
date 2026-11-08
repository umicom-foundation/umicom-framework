#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWebRequestStreamChecks.cmake
#
# PURPOSE:
#   Register incremental request ownership and partition-independence regression scenarios.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-web-request-stream-test "${CMAKE_CURRENT_LIST_DIR}/../tests/web/test_request_stream.c")
    target_link_libraries(umicom-web-request-stream-test PRIVATE Umicom::web)
    umicom_apply_warnings(umicom-web-request-stream-test)
    umicom_apply_sanitizers(umicom-web-request-stream-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-web-request-stream-test)
    endif()
    foreach(case create-invalid initial empty get-single get-bytewise post-bytewise binary-body all-splits many-fragments exact-capacity owned-input capacity full-incomplete declared-overlimit pipeline-later reset-complete reset-failure null-input-preserves null-output busy-output-preserved error-output-preserved snapshot-invalid body-pending direct-read delayed-semantic)
        add_test(NAME framework.web.request_stream.${case} COMMAND umicom-web-request-stream-test ${case})
        set_tests_properties(framework.web.request_stream.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;web;http;regression")
    endforeach()
endif()
