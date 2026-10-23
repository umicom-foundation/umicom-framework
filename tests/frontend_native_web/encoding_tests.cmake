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
# Preserve the published aggregate-only registration for engineering review.
# The active registration below adds named cases and keeps validation ownership.
if(FALSE)
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

endif()

# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Use the canonical helper for warnings and the published aggregate test.
# Keep explicit validation ownership and the former registration for review.
if(FALSE)
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
add_executable(umicom-web-encoding-boundaries-test "${CMAKE_CURRENT_LIST_DIR}/test_encoding_boundaries.c")
target_link_libraries(umicom-web-encoding-boundaries-test PRIVATE Umicom::frontend)
set_target_properties(umicom-web-encoding-boundaries-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-web-encoding-boundaries-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-web-encoding-boundaries-test)
endif()
foreach(_case IN ITEMS buffer-state buffer-boundary buffer-alias batch-count inline-string
        unicode controls atomic frame-capacity wire-format)
    add_test(NAME framework.frontend.native_web.encoding.${_case}
        COMMAND umicom-web-encoding-boundaries-test "${_case}")
    set_tests_properties(framework.frontend.native_web.encoding.${_case} PROPERTIES
        TIMEOUT 20 LABELS "framework;native-web-frontend;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-web-encoding-boundaries-test)
endif()

endif()

include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
if(NOT COMMAND umicom_add_native_web_frontend_test)
    message(FATAL_ERROR "Native-Web encoding tests require the canonical frontend helper")
endif()
umicom_add_native_web_frontend_test(
    umicom-native-web-encoding-boundaries-test
    framework.frontend.native_web.encoding.boundaries
    tests/frontend_native_web/test_encoding_boundaries.c)
set_target_properties(umicom-native-web-encoding-boundaries-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-native-web-encoding-boundaries-test)
endif()
# Retain the former build-target spelling as a dependency-only compatibility target.
add_custom_target(umicom-web-encoding-boundaries-test
    DEPENDS umicom-native-web-encoding-boundaries-test)
foreach(_case IN ITEMS buffer-state buffer-boundary buffer-alias batch-count inline-string
        unicode controls atomic frame-capacity wire-format)
    add_test(NAME framework.frontend.native_web.encoding.${_case}
        COMMAND $<TARGET_FILE:umicom-native-web-encoding-boundaries-test> "${_case}")
    set_tests_properties(framework.frontend.native_web.encoding.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;native-web-frontend;encoding;regression")
endforeach()
set_tests_properties(framework.frontend.native_web.encoding.boundaries PROPERTIES
    TIMEOUT 30 LABELS "framework;native-web-frontend;encoding;regression")
