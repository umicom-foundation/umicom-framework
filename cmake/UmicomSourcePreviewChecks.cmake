#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSourcePreviewChecks.cmake
#
# PURPOSE:
#   Register bounded source-preview regression coverage and the installed SDK exercise.
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
    foreach(_fixture admission failures)
        add_executable(umicom-source-preview-${_fixture}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/test_source_preview_${_fixture}.c")
        target_link_libraries(umicom-source-preview-${_fixture}-test PRIVATE Umicom::browser)
        set_target_properties(umicom-source-preview-${_fixture}-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-source-preview-${_fixture}-test)
        umicom_apply_sanitizers(umicom-source-preview-${_fixture}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-source-preview-${_fixture}-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-source-preview-failures-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/browser/source_preview.c")
    endif()
    foreach(_case htm html-unicode scalar-1 scalar-2 scalar-3 scalar-4 scalar-5 scalar-6 title-unicode html-title exact-span alias empty span-limit span-over embedded-nul source-utf8 path-limit language-limit path-utf8 language-utf8 request-trust request-consent budget-zero budget-over revision-zero null-source stale identity content-utf8 title-utf8 summary-utf8 unterminated length source-length truncated-flag executed-flag kind version size failure unregister trust-consent-only trust-workspace-only trust-both provider-title provider-id provider-language provider-extension provider-execute provider-trust terminated-snapshot over-budget)
        add_test(NAME framework.browser.source_preview.${_case}
            COMMAND umicom-source-preview-admission-test "${_case}")
        set_tests_properties(framework.browser.source_preview.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;browser;preview;unicode;regression;no-execution")
    endforeach()
    add_test(NAME framework.browser.source_preview.allocation
        COMMAND umicom-source-preview-failures-test)
    set_tests_properties(framework.browser.source_preview.allocation PROPERTIES
        TIMEOUT 30 LABELS "framework;browser;preview;fault-injection;regression")
    add_executable(umicom-source-preview-example
        "${CMAKE_CURRENT_LIST_DIR}/../examples/source_preview/main.c")
    target_link_libraries(umicom-source-preview-example PRIVATE Umicom::browser)
    set_target_properties(umicom-source-preview-example PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-source-preview-example)
    umicom_apply_sanitizers(umicom-source-preview-example)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    if(WIN32)
        enable_language(RC)
    endif()
    umicom_apply_application_branding(TARGET umicom-source-preview-example
        PRODUCT_NAME "Umicom Source Preview Example" INTERNAL_NAME "umicom-source-preview-example"
        RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../resources")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-source-preview-example)
    endif()
    foreach(_case unicode htm refusal markdown)
        add_test(NAME framework.browser.source_preview-example.${_case}
            COMMAND umicom-source-preview-example "${_case}")
        set_tests_properties(framework.browser.source_preview-example.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;browser;preview;learning;no-execution")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/source-preview-contracts.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/source_preview/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/source_preview/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/examples/source_preview" COMPONENT Learning)

# Structured ownership extends the existing text API without changing its ABI.
if(BUILD_TESTING)
    add_executable(umicom-structured-source-preview-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/browser/test_structured_source_preview.c")
    target_link_libraries(umicom-structured-source-preview-test PRIVATE Umicom::browser)
    set_target_properties(umicom-structured-source-preview-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-structured-source-preview-test)
    umicom_apply_sanitizers(umicom-structured-source-preview-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-structured-source-preview-test)
    endif()
    foreach(_case md markdown language html-priority plain shortened empty alias invalid-source null-output callback-failure callback-stale custom copied-callback-id install-capacity install-exact install-duplicate legacy)
        add_test(NAME framework.browser.structured_source_preview.${_case}
            COMMAND umicom-structured-source-preview-test "${_case}")
        set_tests_properties(framework.browser.structured_source_preview.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;browser;preview;reading;ownership;regression")
    endforeach()
endif()
