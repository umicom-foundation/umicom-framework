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
