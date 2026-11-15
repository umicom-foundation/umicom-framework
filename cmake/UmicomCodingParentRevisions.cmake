# Umicom Framework
# File: cmake/UmicomCodingParentRevisions.cmake
#
# PURPOSE:
# Register parent observation revisions and callback cancellation boundaries.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-parent-revisions-test tests/ai_coding_runtime/test_platform_revisions.c)
    target_link_libraries(umicom-ai-coding-parent-revisions-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-parent-revisions-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-parent-revisions-test)
    umicom_apply_sanitizers(umicom-ai-coding-parent-revisions-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-parent-revisions-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-parent-revisions-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/platform.c")
    endif()
    foreach(_operation IN ITEMS refresh_repository set_validation_plan detect_validation run enqueue run_next approve apply continue_repair reject)
        foreach(_case IN ITEMS null exhausted zero busy child-busy snapshot-refused)
            set(_name "framework.ai.coding-parent-revisions.admission.${_operation}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-parent-revisions-test admission "${_operation}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation;revision")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-parent-revisions-test)
        endforeach()
    endforeach()
    foreach(_operation IN ITEMS refresh_repository set_validation_plan detect_validation run enqueue run_next approve apply continue_repair reject)
        foreach(_case IN ITEMS success unchanged-failure changed-failure last-revision cancel-failure readback-failure)
            set(_name "framework.ai.coding-parent-revisions.result.${_operation}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-parent-revisions-test result "${_operation}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation;revision")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-parent-revisions-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS null idle zero exhausted active active-last)
        set(_name "framework.ai.coding-parent-revisions.cancel.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-parent-revisions-test cancel "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation;revision")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-parent-revisions-test)
    endforeach()
    foreach(_case IN ITEMS agent.state agent.last_status agent.iteration agent.maximum_iterations agent.context_files agent.patch_files agent.validation_steps agent.validation_failures agent.revision agent.cancelled agent.patch_recorded agent.patch_applied agent.validation_passed agent.rolled_back assistant.revision assistant.repository_files assistant.task_count assistant.patch_count assistant.draft_patches assistant.approved_patches assistant.applied_patches assistant.reverted_patches assistant.patch_policy.maximum_files assistant.patch_policy.maximum_changed_lines assistant.patch_policy.allow_create assistant.patch_policy.allow_delete assistant.patch_policy.require_approval repository.scanned_files repository.indexed_files repository.ignored_non_text_files repository.capacity_dropped_files repository.revision repository.truncated queued_events queued_tasks agent.task_id agent.request_id agent.provider_id agent.model_id agent.patch_id assistant.last_request_id assistant.last_patch_id padding-and-tail parent-revision)
        set(_name "framework.ai.coding-parent-revisions.semantic.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-parent-revisions-test semantic "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation;revision")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-parent-revisions-test)
    endforeach()
endif()
