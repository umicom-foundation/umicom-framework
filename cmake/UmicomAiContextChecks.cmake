# Umicom Framework
# File: cmake/UmicomAiContextChecks.cmake
#
# PURPOSE:
# Register native context ownership, complete planning and privacy admission checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-context-admission-test tests/test_ai_context_admission.c)
    target_link_libraries(umicom-ai-context-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-context-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-context-admission-test)
    umicom_apply_sanitizers(umicom-ai-context-admission-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-context-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/context_broker.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/context_broker_fixture.h")
    endif()
    foreach(_case IN ITEMS valid
            source-id
            label
            uri
            empty-id
            empty-label
            empty-uri
            kind-low
            kind-high
            class-low
            class-high
            zero-tokens
            capacity
            replace-full
            revision-max-upsert
            revision-max-remove
            revision-max-clear
            revision-last
            empty-clear
            legacy-clear-max
            corrupt-count
            corrupt-revision
            corrupt-record-revision
            corrupt-record-ahead
            corrupt-duplicate
            corrupt-text
            capture-null
            remove-query
            missing-remove
            at-missing)
        set(_name "framework.ai.context.admission.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-context-admission-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;privacy")
        # Keep the malformed draft below for review; a placeholder replacement
        # had consumed the CMake TARGETS keyword. The active call retains it.
        # umicom_test_impact_owners("${_name}" context-admissionS umicom-ai-context-admission-test)
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-context-admission-test)
    endforeach()
    add_executable(umicom-ai-context-planning-test tests/test_ai_context_planning.c)
    target_link_libraries(umicom-ai-context-planning-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-context-planning-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-context-planning-test)
    umicom_apply_sanitizers(umicom-ai-context-planning-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-context-planning-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/context_broker.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/context_broker_fixture.h")
    endif()
    foreach(_case IN ITEMS ordering
            mandatory-negative
            enabled-negative
            mandatory-budget
            mandatory-slots
            mandatory-private
            mandatory-approval
            optional-budget
            optional-private
            disabled
            exact-budget
            maximum-tokens
            provider-low
            provider-high
            invalid-policy
            empty-provider
            empty-policy
            null-output
            zero-context
            reserved-equal
            zero-sources
            too-many-sources
            source-copy)
        set(_name "framework.ai.context.planning.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-context-planning-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;privacy")
        # Keep the malformed draft below for review; a placeholder replacement
        # had consumed the CMake TARGETS keyword. The active call retains it.
        # umicom_test_impact_owners("${_name}" context-planningS umicom-ai-context-planning-test)
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-context-planning-test)
    endforeach()
    add_executable(umicom-ai-privacy-admission-test tests/test_ai_privacy_admission.c)
    target_link_libraries(umicom-ai-privacy-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-privacy-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-privacy-admission-test)
    umicom_apply_sanitizers(umicom-ai-privacy-admission-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-privacy-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/context_broker.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai/context_broker_fixture.h")
    endif()
    foreach(_case IN ITEMS valid
            local-low
            local-high
            remote-low
            remote-high
            null-policy
            provider-low
            provider-high
            class-low
            class-high
            decision-matrix
            flags
            service-refusal
            service-valid)
        set(_name "framework.ai.privacy.admission.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-ai-privacy-admission-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;context;privacy")
        # Keep the malformed draft below for review; a placeholder replacement
        # had consumed the CMake TARGETS keyword. The active call retains it.
        # umicom_test_impact_owners("${_name}" privacy-admissionS umicom-ai-privacy-admission-test)
        umicom_test_impact_owners("${_name}" TARGETS umicom-ai-privacy-admission-test)
    endforeach()
endif()
