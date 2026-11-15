# Umicom Framework
# File: cmake/UmicomCodingObservationPublication.cmake
#
# PURPOSE:
# Register coherent observation publication and agent identity boundary regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-observation-publication-test tests/ai_coding_runtime/test_observation_publication.c)
    target_link_libraries(umicom-ai-coding-observation-publication-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-observation-publication-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-observation-publication-test)
    umicom_apply_sanitizers(umicom-ai-coding-observation-publication-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-observation-publication-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-observation-publication-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/agent.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/event_queue.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/history.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/audit.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/observation.c")
    endif()
    foreach(_case IN ITEMS success event-only null-events null-audit null-history null-event null-record full-event bad-event-text bad-audit-text bad-snapshot-text event-head event-count history-count audit-count audit-sequence audit-exhausted audit-last retention)
        set(_name "framework.ai.coding-observation.publication.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-observation-publication-test publication "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-observation-publication-test)
    endforeach()
    foreach(_mode IN ITEMS event state)
        foreach(_case IN ITEMS success full-event corrupt-event corrupt-audit exhausted-audit invalid-task-text invalid-patch-text sequence-exhausted last-sequence revision-exhausted last-revision history-corrupt long-message null-message audit-last retained-history)
            set(_name "framework.ai.coding-observation.agent.${_mode}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-observation-publication-test agent "${_case}" "${_mode}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-observation-publication-test)
        endforeach()
    endforeach()
    add_test(NAME framework.ai.coding-observation.run-full-queue COMMAND umicom-ai-coding-observation-publication-test run-full-queue)
    set_tests_properties(framework.ai.coding-observation.run-full-queue PROPERTIES TIMEOUT 45 LABELS "framework;ai;observation")
    umicom_test_impact_owners(framework.ai.coding-observation.run-full-queue TARGETS umicom-ai-coding-observation-publication-test)
endif()
