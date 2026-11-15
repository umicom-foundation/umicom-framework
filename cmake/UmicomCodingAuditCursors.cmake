# Umicom Framework
# File: cmake/UmicomCodingAuditCursors.cmake
#
# PURPOSE:
# Register bounded audit retention and sequence cursor regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-audit-cursors-test tests/ai_coding_runtime/test_audit_cursors.c)
    target_link_libraries(umicom-ai-coding-audit-cursors-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-audit-cursors-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-audit-cursors-test)
    umicom_apply_sanitizers(umicom-ai-coding-audit-cursors-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-audit-cursors-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-audit-cursors-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/audit.c")
    endif()
    foreach(_case IN ITEMS empty null-record huge zero null-owner null-output null-report future maximum-cursor ordered scalar copy page at-end metadata gap gap-boundary continue replace)
        set(_name "framework.ai.coding-audit-cursors.public.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-audit-cursors-test public "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;audit;cursors")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-audit-cursors-test)
    endforeach()
    foreach(_case IN ITEMS count sequence-low sequence-gap empty-sequence last-identity exhaustion batch-exhaustion stored-text)
        set(_name "framework.ai.coding-audit-cursors.private.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-audit-cursors-test private "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;audit;cursors")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-audit-cursors-test)
    endforeach()
    foreach(_mode IN ITEMS unterminated last-byte empty)
        foreach(_field IN ITEMS task-id summary)
            set(_name "framework.ai.coding-audit-cursors.text.${_mode}.${_field}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-audit-cursors-test text "${_mode}" "${_field}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;audit;cursors")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-audit-cursors-test)
        endforeach()
    endforeach()
endif()
