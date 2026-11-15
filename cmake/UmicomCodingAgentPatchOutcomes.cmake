# Umicom Framework
# File: cmake/UmicomCodingAgentPatchOutcomes.cmake
#
# PURPOSE:
# Register correlated agent patch recovery outcome and lifecycle coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-ai-coding-agent-patch-outcomes-test tests/ai_coding_runtime/test_agent_patch_outcomes.c)
    target_link_libraries(umicom-ai-coding-agent-patch-outcomes-test PRIVATE Umicom::developer)
    set_target_properties(umicom-ai-coding-agent-patch-outcomes-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-agent-patch-outcomes-test)
    umicom_apply_sanitizers(umicom-ai-coding-agent-patch-outcomes-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-agent-patch-outcomes-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-agent-patch-outcomes-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/agent.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/platform.c")
    endif()
    foreach(_direction IN ITEMS apply revert)
        foreach(_case IN ITEMS success primary-failure recovery-failure readback-failure admission-refusal notification-failure previous-during invalid-snapshot)
            set(_name "framework.ai.coding-agent-outcome.flow.${_direction}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-agent-patch-outcomes-test flow "${_case}" "${_direction}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;recovery;observation")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-agent-patch-outcomes-test)
        endforeach()
    endforeach()
    foreach(_case IN ITEMS null-platform null-agent null-output missing corrupt-task corrupt-patch copy-isolation active-read)
        set(_name "framework.ai.coding-agent-outcome.read.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-agent-patch-outcomes-test read "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;recovery;observation")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-agent-patch-outcomes-test)
    endforeach()
    foreach(_case IN ITEMS empty-task busy-run admitted-reset)
        set(_name "framework.ai.coding-agent-outcome.lifecycle.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-coding-agent-patch-outcomes-test lifecycle "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;recovery;observation")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-agent-patch-outcomes-test)
    endforeach()
endif()
