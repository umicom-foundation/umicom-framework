#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTaskGraphChecks.cmake
#
# PURPOSE:
#   Register task graph transaction and lifecycle policy regression coverage.
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
    foreach(suite transactions policy)
        add_executable(umicom-task-graph-${suite}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/build/test_task_graph_${suite}.c")
        target_link_libraries(umicom-task-graph-${suite}-test PRIVATE Umicom::build)
        set_target_properties(umicom-task-graph-${suite}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-task-graph-${suite}-test)
        umicom_apply_sanitizers(umicom-task-graph-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-task-graph-${suite}-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-task-graph-transactions-test FILES "${CMAKE_CURRENT_LIST_DIR}/../src/build/task_graph.c")
    endif()
    set(transactions_cases rebuild-missing rebuild-cycle rebuild-allocation rebuild-revision reset-graph reset-node refresh-graph refresh-node start-graph start-node progress-graph progress-node complete-graph complete-node complete-downstream fail-graph fail-node cancel-graph cancel-node retry-graph retry-node skip-graph skip-node cancel-waiting-graph cancel-waiting-node refresh-missing finish-missing dependency-count-revision unchanged-terminal-limit)
    set(policy_cases empty chain-success reverse-order diamond missing cycle disabled-missing disabled-cycle continue-on-error failure-blocks retry attempt-limit cancel-waiting skip-unblocks progress-bounds nonzero-complete reset-after-failure owned-source aggregate-progress invalid-state)
    foreach(suite transactions policy)
        foreach(case IN LISTS ${suite}_cases)
            add_test(NAME framework.task_graph.${suite}.${case} COMMAND umicom-task-graph-${suite}-test "${case}")
            set_tests_properties(framework.task_graph.${suite}.${case} PROPERTIES TIMEOUT 20 LABELS "framework;build;task;graph;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.task_graph.${suite}.${case} TARGETS umicom-task-graph-${suite}-test)
            endif()
        endforeach()
    endforeach()
endif()
