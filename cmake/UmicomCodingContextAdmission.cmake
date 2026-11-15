# Umicom Framework
# File: cmake/UmicomCodingContextAdmission.cmake
#
# PURPOSE:
# Register bounded repository metadata and complete coding context plan checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-coding-context-admission-test tests/test_ai_coding_context_admission.c)
    target_link_libraries(umicom-ai-coding-context-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-coding-context-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-context-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-context-admission-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-context-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/coding_context.c")
    endif()
    set(_owner_cases valid
        path-bound
        language-bound
        summary-bound
        empty-path
        optional-empty
        unsafe-paths
        zero-tokens
        zero-priority
        class-low
        class-high
        null-source
        capacity
        replace-full
        revision-max-upsert
        revision-max-remove
        revision-max-clear
        revision-last
        empty-clear
        legacy-clear-max
        remove-valid
        remove-query
        missing-remove
        at-missing
        capture-null
        corrupt-count
        corrupt-revision
        corrupt-record-zero
        corrupt-record-ahead
        corrupt-duplicate
        corrupt-text)
    set(_plan_cases ordering
        active-negative
        enabled-negative
        zero-budget
        exact-budget
        optional-private
        one-slot
        all-disabled
        maximum-tokens
        provider-low
        provider-high
        invalid-policy
        empty-provider
        empty-policy
        null-output
        null-policy
        zero-files
        too-many-files
        source-copy)
    foreach(_group IN ITEMS owner plan)
        foreach(_case IN LISTS _${_group}_cases)
            set(_name "framework.ai.coding-context.${_group}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-context-admission-test "${_group}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;coding-context")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-context-admission-test)
        endforeach()
    endforeach()
endif()
