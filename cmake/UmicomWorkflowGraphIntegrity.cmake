# Umicom Framework
# File: cmake/UmicomWorkflowGraphIntegrity.cmake
#
# PURPOSE:
# Register whole-graph admission, deterministic planning and retained-output regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET Umicom::integration)
    add_executable(umicom-workflow-graph-integrity-test tests/integration_fabric/test_workflow_graph_integrity.c)
    add_executable(umicom-workflow-plan-integrity-test tests/integration_fabric/test_workflow_plan_integrity.c)
    foreach(_target IN ITEMS umicom-workflow-graph-integrity-test umicom-workflow-plan-integrity-test)
        target_link_libraries(${_target} PRIVATE Umicom::integration)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_graph.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_plan.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/workflow_step.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/integration/fabric/types.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/base/value_construction_internal.h")
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case IN ITEMS empty-null
            counts
            step-text
            step-domain
            edge-text
            missing-endpoints
            late-invalid
            cycles
            refused-edge
            borrowed-input
            duplicate-edge
            step-capacity
            chain-capacity
            edge-capacity)
        add_test(NAME framework.workflow.graph_integrity.${_case}
            COMMAND umicom-workflow-graph-integrity-test ${_case})
        set_tests_properties(framework.workflow.graph_integrity.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;workflow;graph;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.workflow.graph_integrity.${_case}
                TARGETS umicom-workflow-graph-integrity-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS deterministic
            invalid-storage
            cycle-empty-null
            allocation
            duplicate-edges
            full-chain
            maximum-id
            fingerprint-scope)
        add_test(NAME framework.workflow.plan_integrity.${_case}
            COMMAND umicom-workflow-plan-integrity-test ${_case})
        set_tests_properties(framework.workflow.plan_integrity.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;workflow;plan;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.workflow.plan_integrity.${_case}
                TARGETS umicom-workflow-plan-integrity-test)
        endif()
    endforeach()
endif()
