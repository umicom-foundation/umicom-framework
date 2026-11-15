# Umicom Framework
# File: cmake/UmicomCodingTaskAdmission.cmake
#
# PURPOSE:
# Register canonical provider and complete coding task admission checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-coding-task-admission-test tests/test_ai_coding_task_admission.c)
    target_link_libraries(umicom-ai-coding-task-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-coding-task-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-task-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-task-admission-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-task-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/coding_assistant.c")
    endif()
    set(_request_cases public
        restricted
        class-low
        class-high
        instruction-bound
        runtime-bound
        selection
        zero-budget
        file-limit
        unsafe-path
        null)
    set(_prepare_cases remote-public
        remote-internal
        remote-as-local
        remote-provider-denied
        remote-approval
        authorengine
        test-provider
        provider-low
        provider-high
        local-as-remote
        sensitive-denied
        sensitive-approved
        class-low
        class-high
        runtime-missing
        owner-zero
        owner-max
        owner-last
        task-count
        patch-count
        zero-repository-budget
        mandatory-general-denied
        history-full
        valid
        borrow-output
        null-output
        null-request
        null-service)
    foreach(_group IN ITEMS request prepare)
        foreach(_case IN LISTS _${_group}_cases)
            set(_name "framework.ai.coding-task.${_group}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-admission-test "${_group}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;coding-task")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-admission-test)
        endforeach()
    endforeach()
endif()
