#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeveloperLifecycleExecution.cmake
#
# PURPOSE:
#   Register controlled lifecycle execution and native command admission regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    foreach(_suite execution workflow_cli)
        add_executable(umicom-lifecycle-${_suite}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_lifecycle/test_${_suite}.c")
        target_link_libraries(umicom-lifecycle-${_suite}-test PRIVATE Umicom::developer)
        set_target_properties(umicom-lifecycle-${_suite}-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-lifecycle-${_suite}-test)
        umicom_apply_sanitizers(umicom-lifecycle-${_suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-lifecycle-${_suite}-test)
        endif()
    endforeach()
    # The CLI inclusion fixture shares identifiers with production source.
    # Treat a future collision as a failed regression build, not a quiet warning.
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-lifecycle-workflow_cli-test PRIVATE -Werror=shadow)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-lifecycle-workflow_cli-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_workflow.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
    endif()
    set(_execution_cases complete verify build test publish update preview preview-no-host strict negative-backend unused-host-slot null-request null-report null-host missing-message blank-message empty-message source-null source-empty source-option goal-zero goal-range jobs-zero jobs-negative jobs-plus jobs-fraction jobs-overflow jobs-empty preset-empty remote-option branch-empty strict-range preview-range submodules-range submodules-scope strict-update preset-scope jobs-scope message-scope remote-scope branch-scope fail-1 fail-2 fail-3 fail-4 fail-5 fail-6 fail-7 fail-8 fail-9 missing-1 missing-2 missing-3 missing-4 missing-5 missing-6 missing-7 missing-8 missing-9)
    set(_workflow_cli_cases complete full-options verify build test publish update update-options plan dry-run help empty negative null-vector null-command null-option null-value empty-value missing-value unknown-option unknown-command goal-on-complete goal-plan goal-unknown missing-message blank-message jobs-zero jobs-overflow preset-on-publish submodules-on-complete strict-on-update null-context fail-1 fail-2 fail-3 fail-4 fail-5 fail-6 fail-7 fail-8 fail-9 duplicate-0 duplicate-1 duplicate-2 duplicate-3 duplicate-4 duplicate-5 duplicate-6 duplicate-7 duplicate-8 duplicate-9)
    foreach(_suite execution workflow_cli)
        foreach(_case IN LISTS _${_suite}_cases)
            add_test(NAME "framework.developer.lifecycle_${_suite}.${_case}"
                COMMAND umicom-lifecycle-${_suite}-test "${_case}")
            set_tests_properties("framework.developer.lifecycle_${_suite}.${_case}" PROPERTIES
                TIMEOUT 20 LABELS "framework;developer;workflow;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.developer.lifecycle_${_suite}.${_case}"
                    TARGETS umicom-lifecycle-${_suite}-test)
            endif()
        endforeach()
    endforeach()
endif()
