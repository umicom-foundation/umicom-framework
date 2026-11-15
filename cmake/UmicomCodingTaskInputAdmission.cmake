# Umicom Framework
# File: cmake/UmicomCodingTaskInputAdmission.cmake
#
# PURPOSE:
# Register bounded task construction and execution entry preservation checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-task-input-test tests/ai_coding_runtime/test_task_input_admission.c)
    target_link_libraries(umicom-ai-coding-task-input-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-task-input-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-task-input-test)
    umicom_apply_sanitizers(umicom-ai-coding-task-input-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-task-input-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-task-input-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/agent.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/platform.c")
    endif()
    foreach(_entry IN ITEMS value agent enqueue)
        foreach(_case IN ITEMS null-id null-request empty-id overlong-id unterminated-id
            request_id session_id runtime_id workspace_root active_path language_id instruction
            normal maximum-id draft)
            set(_name "framework.ai.coding-task-input.entry.${_entry}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-input-test entry "${_entry}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;admission;ownership")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-input-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS value agent)
        set(_name "framework.ai.coding-task-input.alias.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-input-test alias "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;admission;ownership")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-input-test)
    endforeach()
    foreach(_case IN ITEMS request_id session_id runtime_id workspace_root active_path language_id instruction)
        set(_name "framework.ai.coding-task-input.boundary.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-input-test boundary "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;admission;ownership")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-input-test)
    endforeach()
    add_test(NAME framework.ai.coding-task-input.null-output COMMAND umicom-ai-coding-task-input-test null-output)
    set_tests_properties(framework.ai.coding-task-input.null-output PROPERTIES TIMEOUT 45 LABELS "framework;ai;admission;ownership")
    umicom_test_impact_owners(framework.ai.coding-task-input.null-output TARGETS umicom-ai-coding-task-input-test)
endif()
