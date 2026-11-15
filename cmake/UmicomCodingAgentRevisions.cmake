# Umicom Framework
# File: cmake/UmicomCodingAgentRevisions.cmake
#
# PURPOSE:
# Register agent task identity and terminal revision headroom regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-agent-revisions-test tests/ai_coding_runtime/test_agent_revisions.c)
    target_link_libraries(umicom-ai-coding-agent-revisions-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-agent-revisions-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-agent-revisions-test)
    umicom_apply_sanitizers(umicom-ai-coding-agent-revisions-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-agent-revisions-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-agent-revisions-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/agent.c")
    endif()
    foreach(_state IN ITEMS idle preparing context generating review applying validating repairing completed cancelled failed)
        foreach(_case IN ITEMS ordinary last-headroom final-slot exhausted zero full-final-slot)
            set(_name "framework.ai.coding-agent-revisions.transition.${_state}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-agent-revisions-test transition "${_state}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;revision;lifecycle")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-agent-revisions-test)
        endforeach()
    endforeach()
    foreach(_state IN ITEMS idle completed failed cancelled)
        foreach(_case IN ITEMS ordinary last-admissible one-left exhausted zero full-queue cancelled)
            set(_name "framework.ai.coding-agent-revisions.run.${_state}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-agent-revisions-test run "${_state}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;revision;lifecycle")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-agent-revisions-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS successive-tasks invalid-input busy-run rejected-followup apply-final-slot repair-final-slot cancel-final-slot)
        set(_name "framework.ai.coding-agent-revisions.lifecycle.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-agent-revisions-test lifecycle "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;revision;lifecycle")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-agent-revisions-test)
    endforeach()
endif()
