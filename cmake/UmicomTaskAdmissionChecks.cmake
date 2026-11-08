#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTaskAdmissionChecks.cmake
#
# PURPOSE:
#   Register task descriptor admission and owned registry regression cases.
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
    foreach(suite admission registry)
        add_executable(umicom-task-${suite}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/build/test_task_${suite}.c")
        target_link_libraries(umicom-task-${suite}-test PRIVATE Umicom::build)
        set_target_properties(umicom-task-${suite}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-task-${suite}-test)
        umicom_apply_sanitizers(umicom-task-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-task-${suite}-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-task-registry-test FILES "${CMAKE_CURRENT_LIST_DIR}/../src/build/task.c")
    endif()
    set(admission_cases valid metadata-size metadata-version dependency-count input-count output-count environment-count scalar-text dependency-text input-text output-text environment-text invalid-kind invalid-run-policy invalid-phase invalid-flags invalid-has-phase invalid-incremental zero-attempts self-dependency duplicate-dependency empty-input duplicate-input empty-output duplicate-output environment-no-equals environment-empty-name duplicate-environment incomplete-builder all-mutators-malformed command-too-long directory-too-long command-alias directory-retained mutator-revision-limit maximum-entries unused-entry-ignored)
    set(registry_cases create-null new-owner owned-copy replace-order remove-middle missing-preserves capacity-preserves insert-revision-limit replace-revision-limit remove-revision-limit reset-revision-limit clear-revision-limit replacement-task-limit zero-task-revision reset-normal null-reset)
    foreach(suite admission registry)
        foreach(case IN LISTS ${suite}_cases)
            add_test(NAME framework.task.${suite}.${case} COMMAND umicom-task-${suite}-test "${case}")
            set_tests_properties(framework.task.${suite}.${case} PROPERTIES TIMEOUT 20 LABELS "framework;build;task;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.task.${suite}.${case} TARGETS umicom-task-${suite}-test)
            endif()
        endforeach()
    endforeach()
endif()
