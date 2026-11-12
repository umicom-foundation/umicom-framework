#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWebResponseBodyChecks.cmake
#
# PURPOSE:
#   Register native owned-body and canonical-asset regression cases without a network listener.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND (WIN32 OR UNIX) AND NOT EMSCRIPTEN)
    add_executable(umicom-web-response-body-test "${CMAKE_CURRENT_LIST_DIR}/../tests/web/test_response_body.c")
    target_link_libraries(umicom-web-response-body-test PRIVATE Umicom::web)
    umicom_apply_warnings(umicom-web-response-body-test)
    umicom_apply_sanitizers(umicom-web-response-body-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-web-response-body-test)
    endif()
    get_filename_component(_brand_root "${CMAKE_CURRENT_LIST_DIR}/../resources/brand" ABSOLUTE)
    foreach(_case IN ITEMS ownership headers svg ico head empty exact-limit oversized traversal missing method legacy fallback-clear gate exact-route write-failure partial-failure cancel-body body-failure body-conflict invalid-header no-content not-modified preview preview-gate)
        add_test(NAME framework.web.response_body.${_case}
            COMMAND umicom-web-response-body-test ${_case} "${_brand_root}")
        set_tests_properties(framework.web.response_body.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;web;http;branding;regression")
    endforeach()
endif()
