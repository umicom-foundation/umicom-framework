# Umicom Framework
# File: cmake/UmicomCodingContextSummaryAdmission.cmake
#
# PURPOSE:
# Register metadata summary bounds and publication compatibility checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-context-summary-admission-test tests/ai_coding_runtime/test_context_summary_admission.c)
    target_link_libraries(umicom-ai-coding-context-summary-admission-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-context-summary-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-context-summary-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-context-summary-admission-test)
    foreach(_case IN ITEMS null count path empty-path unsafe-path language language-newline path-del path-utf8 language-utf8)
        set(_name "framework.ai.coding-context-summary-admission.invalid.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-context-summary-admission-test invalid "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;summary")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-context-summary-admission-test)
    endforeach()
    foreach(_case IN ITEMS exact short one zero null-output alias)
        set(_name "framework.ai.coding-context-summary-admission.capacity.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-context-summary-admission-test capacity "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;summary")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-context-summary-admission-test)
    endforeach()
    foreach(_case IN ITEMS metadata-only empty empty-language unicode flags numeric-maxima maximum-labels)
        set(_name "framework.ai.coding-context-summary-admission.success.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-context-summary-admission-test success "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;summary")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-context-summary-admission-test)
    endforeach()
endif()
