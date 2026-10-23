#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/frontend_native_web/encoding_tests.cmake
#
# PURPOSE:
#   Register additional native C23 HTML/text encoding-boundary regressions.
#   This file is included by UmicomNativeWebFrontendPlatform.cmake after the
#   single canonical native-Web test helper has been defined. No test is
#   registered unless BUILD_TESTING is enabled.
#
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT BUILD_TESTING)
    return()
endif()
if(NOT COMMAND umicom_add_native_web_frontend_test)
    message(FATAL_ERROR "Native-Web encoding tests require the canonical frontend test helper")
endif()

umicom_add_native_web_frontend_test(
    umicom-native-web-encoding-boundaries-test
    framework.frontend.native_web.encoding.boundaries
    tests/frontend_native_web/test_encoding_boundaries.c
)
set_tests_properties(framework.frontend.native_web.encoding.boundaries PROPERTIES
    LABELS "framework;native-web-frontend;encoding;regression"
    TIMEOUT 30
)
