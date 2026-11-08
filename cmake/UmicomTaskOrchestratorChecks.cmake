#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTaskOrchestratorChecks.cmake
#
# PURPOSE:
#   Register orchestrator transactions and dependency-closure regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(suite transactions closure)
        add_executable(umicom-task-orchestrator-${suite}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/build/test_task_orchestrator_${suite}.c")
        target_link_libraries(umicom-task-orchestrator-${suite}-test PRIVATE Umicom::build)
        set_target_properties(umicom-task-orchestrator-${suite}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-task-orchestrator-${suite}-test)
        umicom_apply_sanitizers(umicom-task-orchestrator-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-task-orchestrator-${suite}-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-task-orchestrator-transactions-test FILES "${CMAKE_CURRENT_LIST_DIR}/../src/build/task_orchestrator.c" "${CMAKE_CURRENT_LIST_DIR}/../src/build/task_graph.c")
    endif()
    set(transactions_cases plan-missing plan-cycle plan-allocation operation-too-long requested-too-long plan-generation plan-revision plan-graph begin-revision start-revision progress-revision complete-revision complete-graph fail-revision retry-revision retry-count stop-revision stop-graph finish-revision finish-idle finish-planned finish-active finish-waiting plan-running plan-alias)
    set(closure_cases deep-chain deep-cycle diamond-once selected-only all-enabled disabled-dependency source-edit parallel-bound stop-running retry-failed empty-begin graph-address)
    foreach(suite transactions closure)
        foreach(case IN LISTS ${suite}_cases)
            add_test(NAME framework.task_orchestrator.${suite}.${case} COMMAND umicom-task-orchestrator-${suite}-test "${case}")
            set_tests_properties(framework.task_orchestrator.${suite}.${case} PROPERTIES TIMEOUT 20 LABELS "framework;build;task;orchestrator;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.task_orchestrator.${suite}.${case} TARGETS umicom-task-orchestrator-${suite}-test)
            endif()
        endforeach()
    endforeach()
endif()
