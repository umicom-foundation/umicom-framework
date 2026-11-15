# Umicom Framework
# File: cmake/UmicomAiContextObservation.cmake
#
# PURPOSE:
# Register owner-run context observation and stale preview regression scenarios.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-context-observation-test tests/test_ai_context_observation.c)
    target_link_libraries(umicom-ai-context-observation-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-context-observation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-context-observation-test)
    umicom_apply_sanitizers(umicom-ai-context-observation-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-context-observation-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/authorengine_service.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/runtime_catalogue_fixture.h")
    endif()
    foreach(_case IN ITEMS valid
            borrowed-id
            policy-stale
            context-upsert
            context-remove
            context-clear
            runtime-replace
            runtime-health
            runtime-missing
            unrelated-runtime
            failed-policy-update
            failed-context-update
            failed-runtime-update
            readonly-exhausted
            invalid-owner-zero
            refusal-budget
            refusal-privacy
            refusal-provider
            missing-runtime
            null-service
            null-id
            null-plan
            null-observation
            check-null-service
            check-null-observation
            check-empty-id
            check-id-bound
            check-zero-revisions
            refresh)
        set(_name "framework.ai.context.observation.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-context-observation-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;observation")
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-context-observation-test)
    endforeach()
endif()
