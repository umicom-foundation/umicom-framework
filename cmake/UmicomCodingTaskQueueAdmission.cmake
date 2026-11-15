# Umicom Framework
# File: cmake/UmicomCodingTaskQueueAdmission.cmake
#
# PURPOSE:
# Register bounded draft storage and atomic coding-task batch qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-task-queue-admission-test tests/ai_coding_runtime/test_task_queue_admission.c)
    target_link_libraries(umicom-ai-coding-task-queue-admission-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-task-queue-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-task-queue-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-task-queue-admission-test)
    add_executable(umicom-ai-coding-request-text-test tests/ai_coding_runtime/test_request_text.c)
    target_link_libraries(umicom-ai-coding-request-text-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-request-text-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-request-text-test)
    umicom_apply_sanitizers(umicom-ai-coding-request-text-test)
    foreach(_case IN ITEMS task-id request-id session-id runtime-id workspace active-path language instruction empty-id null-task)
        set(_name "framework.ai.coding-task-queue.reject.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-queue-admission-test reject "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;task-queue;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-queue-admission-test)
    endforeach()
    foreach(_case IN ITEMS empty huge duplicates copy clear capacity full batch wrap)
        set(_name "framework.ai.coding-task-queue.fifo.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-queue-admission-test fifo "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;task-queue;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-queue-admission-test)
    endforeach()
    foreach(_case IN ITEMS last-identity batch-overflow exhausted clear-exhausted head count)
        set(_name "framework.ai.coding-task-queue.invariant.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-queue-admission-test invariant "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;task-queue;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-queue-admission-test)
    endforeach()
    foreach(_case IN ITEMS empty null-owner null-result)
        set(_name "framework.ai.coding-task-queue.output.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-task-queue-admission-test output "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;task-queue;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-task-queue-admission-test)
    endforeach()
    foreach(_mode IN ITEMS unterminated last-byte empty)
        foreach(_field IN ITEMS request-id session-id runtime-id workspace active-path language instruction)
            set(_name "framework.ai.coding-request-text.${_mode}.${_field}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-request-text-test "${_mode}" "${_field}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;drafts;admission")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-request-text-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS null draft opaque)
        set(_name "framework.ai.coding-request-text.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-request-text-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;drafts;admission")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-request-text-test)
    endforeach()
endif()
