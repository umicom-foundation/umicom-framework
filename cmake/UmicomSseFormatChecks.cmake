#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSseFormatChecks.cmake
#
# PURPOSE:
# Register bounded server-sent event formatting regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    add_executable(umicom-sse-format-test "${CMAKE_CURRENT_LIST_DIR}/../tests/web/test_sse_format.c")
    target_link_libraries(umicom-sse-format-test PRIVATE Umicom::web)
    target_compile_features(umicom-sse-format-test PRIVATE c_std_23)
    umicom_apply_warnings(umicom-sse-format-test)
    umicom_apply_sanitizers(umicom-sse-format-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-sse-format-test)
    endif()
    foreach(_case IN ITEMS single-line empty lf cr crlf trailing-lf trailing-crlf leading-lf consecutive colon unicode id-max all-newlines max-fields capacity-sweep null-event null-output unterminated-event unterminated-data event-lf event-cr invalid-event-utf8 invalid-data-utf8 alias-data alias-record optional-length)
        add_test(NAME "framework.web.sse_format.${_case}" COMMAND umicom-sse-format-test "${_case}")
        set_tests_properties("framework.web.sse_format.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;web;sse;protocol")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.web.sse_format.${_case}" TARGETS umicom-sse-format-test)
        endif()
    endforeach()
endif()
