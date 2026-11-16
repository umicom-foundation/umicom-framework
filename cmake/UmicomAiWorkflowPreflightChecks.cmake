# Umicom Framework
# File: cmake/UmicomAiWorkflowPreflightChecks.cmake
#
# PURPOSE:
# Register real typed workflow and allocation regressions with production source ownership.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET Umicom::ai_workflow_preflight)
    add_executable(umicom-ai-workflow-preflight-test tests/ai_workflow/test_preflight.c)
    add_executable(umicom-ai-workflow-allocation-test tests/ai_workflow/test_allocation.c)
    foreach(_target umicom-ai-workflow-preflight-test umicom-ai-workflow-allocation-test)
        target_link_libraries(${_target} PRIVATE Umicom::ai_workflow_preflight)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/src/ai_workflow/preflight.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/ai_workflow/diagnostic.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_graph.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_plan.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_step.c")
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case IN ITEMS valid_chain
            dependency_before_alphabetic
            missing_node
            missing_source_port
            missing_target_port
            type_mismatch
            duplicate_binding
            external_link_conflict
            required_input
            optional_input
            external_input
            self_cycle
            cycle
            unselected_cycle
            too_many_nodes
            too_many_links
            too_many_types
            too_many_ports
            too_many_outputs
            empty_graph
            zero_outputs
            null_link_storage
            null_type_storage
            null_node_storage
            null_output_storage
            null_description
            null_plan
            null_diagnostic
            unterminated_identifier
            unsafe_identifier
            duplicate_node_id
            duplicate_operation_id
            duplicate_port_name
            unknown_value_kind
            unknown_type_index
            invalid_supplied_mask
            effect_denied
            effect_allowed
            unknown_policy_bit
            unknown_effect_definition
            unavailable_provider
            inactive_unavailable
            inactive_required_input
            inactive_effect
            invalid_output_index
            duplicate_output
            multiple_outputs
            parallel_port_links
            deterministic_order
            capacity_chain
            maximum_identifier
            unused_slots
            diagnostic_text)
        add_test(NAME framework.ai_workflow.preflight.${_case}
            COMMAND umicom-ai-workflow-preflight-test "${_case}")
        set_tests_properties(framework.ai_workflow.preflight.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;ai-workflow;preflight;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai_workflow.preflight.${_case}
                TARGETS umicom-ai-workflow-preflight-test)
        endif()
    endforeach()
    foreach(_case workspace planner cleanup)
        add_test(NAME framework.ai_workflow.allocation.${_case}
            COMMAND umicom-ai-workflow-allocation-test "${_case}")
        set_tests_properties(framework.ai_workflow.allocation.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;ai-workflow;allocation;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai_workflow.allocation.${_case}
                TARGETS umicom-ai-workflow-allocation-test)
        endif()
    endforeach()
endif()

# Lifetime and peak planning extends the same SDK target. Keep prior graph tests
# unchanged and bind the new checks to the actual producer sources.
if(BUILD_TESTING AND TARGET Umicom::ai_workflow_preflight)
    add_executable(umicom-ai-workflow-memory-test tests/ai_workflow/test_memory_plan.c)
    add_executable(umicom-ai-workflow-memory-allocation-test tests/ai_workflow/test_memory_allocation.c)
    add_executable(umicom-ai-workflow-memory-example-test examples/ai_workflow_memory/main.c)
    foreach(_target umicom-ai-workflow-memory-test umicom-ai-workflow-memory-allocation-test
            umicom-ai-workflow-memory-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::ai_workflow_preflight)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/src/ai_workflow/memory_plan.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/include/umicom/ai_workflow/memory_plan.h"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/ai_workflow/preflight.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_graph.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_plan.c")
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    umicom_apply_application_branding(TARGET umicom-ai-workflow-memory-example-test
        PRODUCT_NAME "Umicom Workflow Memory" INTERNAL_NAME "umicom-workflow-memory"
        APPLICATION_ID "org.umicom.sdk.workflow.memory"
        VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
    foreach(_case IN ITEMS chain
            exact-limit
            over-limit
            zero-cost
            zero-limit
            resident-only
            retained-result
            selected-ancestor
            fanout
            multiple-input-links
            dead-port
            inactive-unknown
            selected-unknown-output
            unknown-scratch
            unknown-resident
            unknown-with-bytes
            unknown-scratch-with-bytes
            unknown-resident-with-bytes
            unused-cost-slot
            invalid-mask
            cost-count
            null-costs
            null-policy
            null-description
            null-output
            null-diagnostic
            output-sum-overflow
            live-overflow
            scratch-overflow
            maximum-integer
            invalid-graph
            denied-effect
            allowed-effect
            unavailable
            maximum-chain
            copied-lifetime
            repeated)
        add_test(NAME framework.ai_workflow.memory.${_case}
            COMMAND umicom-ai-workflow-memory-test "${_case}")
        set_tests_properties(framework.ai_workflow.memory.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;ai-workflow;memory;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai_workflow.memory.${_case} TARGETS umicom-ai-workflow-memory-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS workspace
            preflight
            cost
            overflow
            success
            repeated
            arguments)
        add_test(NAME framework.ai_workflow.memory_allocation.${_case}
            COMMAND umicom-ai-workflow-memory-allocation-test "${_case}")
        set_tests_properties(framework.ai_workflow.memory_allocation.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;ai-workflow;memory;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai_workflow.memory_allocation.${_case} TARGETS umicom-ai-workflow-memory-allocation-test)
        endif()
    endforeach()
    add_test(NAME framework.ai_workflow.memory_example.complete COMMAND umicom-ai-workflow-memory-example-test)
    add_test(NAME framework.ai_workflow.memory_example.incomplete COMMAND umicom-ai-workflow-memory-example-test --unknown-scratch)
    set_tests_properties(framework.ai_workflow.memory_example.complete framework.ai_workflow.memory_example.incomplete
        PROPERTIES TIMEOUT 20 LABELS "framework;sdk;ai-workflow;memory")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.ai_workflow.memory_example.complete TARGETS umicom-ai-workflow-memory-example-test)
        umicom_test_impact_owners(framework.ai_workflow.memory_example.incomplete TARGETS umicom-ai-workflow-memory-example-test)
    endif()
endif()
