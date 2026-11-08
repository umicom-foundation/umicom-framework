#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeploymentSafetyChecks.cmake
#
# PURPOSE:
#   Register bounded deployment admission and lifecycle regressions without running external providers.
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
    foreach(suite admission lifecycle)
        add_executable(umicom-deployment-${suite}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/build/test_deployment_${suite}.c")
        target_link_libraries(umicom-deployment-${suite}-test PRIVATE Umicom::build)
        set_target_properties(umicom-deployment-${suite}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-deployment-${suite}-test)
        umicom_apply_sanitizers(umicom-deployment-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-deployment-${suite}-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-deployment-lifecycle-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/build/deployment_controller.c")
    endif()
    set(admission_cases valid count-validate count-snapshot count-step count-add plan-text step-text artifact-text target-text plan-metadata step-metadata artifact-metadata target-metadata plan-bool step-bool plan-strategy step-kind step-state plan-revision-zero step-revision-zero order duplicate rollback-gate optional-gate install-before-validation promote-before-install missing-gate invalid-compose-preserves disabled-target-preserves invalid-approve-preserves add-revokes-approval bind-revokes-approval compose-revision bind-revision add-revision approve-revision compose-generation step-out-of-range partial-snapshot source-copy rollback-install-allowed)
    set(lifecycle_cases normal-completion rollback-completion rollback-failure next-busy cancel-idle cancel-running failed-step approval-required load-owned malformed-load load-revision load-step-revision begin-revision start-revision start-step-revision complete-revision complete-step-revision fail-revision fail-step-revision stop-revision stop-step-revision rollback-revision rollback-step-revision finish-rollback-revision)
    foreach(suite admission lifecycle)
        foreach(case IN LISTS ${suite}_cases)
            add_test(NAME framework.deployment.${suite}.${case} COMMAND umicom-deployment-${suite}-test "${case}")
            set_tests_properties(framework.deployment.${suite}.${case} PROPERTIES TIMEOUT 20 LABELS "framework;build;deployment;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.deployment.${suite}.${case} TARGETS umicom-deployment-${suite}-test)
            endif()
        endforeach()
    endforeach()
endif()
