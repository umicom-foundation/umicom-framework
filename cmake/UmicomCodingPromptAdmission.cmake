# Umicom Framework
# File: cmake/UmicomCodingPromptAdmission.cmake
#
# PURPOSE:
# Register native prompt provenance, feedback and failure-publication checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-prompt-admission-test tests/ai_coding_runtime/test_prompt_admission.c)
    target_link_libraries(umicom-ai-coding-prompt-admission-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-prompt-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-prompt-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-prompt-admission-test)
    foreach(_case IN ITEMS null-plan null-context null-config not-ready request-id instruction task active-path selection plan-count priority plan-duplicate temperature context-count context-length context-hash revision unselected-path tokens language active unaccounted-skip)
        set(_name "framework.ai.coding-prompt-admission.invalid.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-admission-test invalid "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-admission-test)
    endforeach()
    foreach(_case IN ITEMS span null-empty null-length embedded-nul size-max alias legacy-limit message-limit)
        set(_name "framework.ai.coding-prompt-admission.feedback.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-admission-test feedback "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-admission-test)
    endforeach()
    foreach(_case IN ITEMS refactor explain selection empty-file chunked skipped zero-files)
        set(_name "framework.ai.coding-prompt-admission.success.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-admission-test success "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-admission-test)
    endforeach()
    foreach(_case IN ITEMS messages path-name)
        set(_name "framework.ai.coding-prompt-admission.capacity.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-prompt-admission-test capacity "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;prompt;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-prompt-admission-test)
    endforeach()
endif()
