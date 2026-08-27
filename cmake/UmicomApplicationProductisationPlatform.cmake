#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationProductisationPlatform.cmake
#
# PURPOSE:
#   Attach Framework-owned portfolio inventory, coverage evidence, shared-gap
#   analysis and completion planning to the canonical application target.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_APPLICATION_PRODUCTISATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "Application Productisation requires the canonical umicom_application target")
endif()

target_sources(umicom_application PRIVATE
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/types.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/inventory.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/capability_matrix.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/evidence.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/coverage.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/gap_analysis.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/completion_plan.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/release_gate.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/command_catalogue.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/runtime.c"
)

if(BUILD_TESTING)
    function(umicom_add_application_productisation_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}"
            "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::application)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;application;productisation;portfolio;acceptance")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${target}")
        endif()
    endfunction()

    umicom_add_application_productisation_test(
        umicom-application-productisation-types-test
        framework.application_productisation.types
        tests/application_productisation/test_types.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-inventory-test
        framework.application_productisation.inventory
        tests/application_productisation/test_inventory.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-capability-matrix-test
        framework.application_productisation.capability_matrix
        tests/application_productisation/test_capability_matrix.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-evidence-test
        framework.application_productisation.evidence
        tests/application_productisation/test_evidence.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-coverage-test
        framework.application_productisation.coverage
        tests/application_productisation/test_coverage.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-gap-plan-test
        framework.application_productisation.gap_plan
        tests/application_productisation/test_gap_plan.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-release-gate-test
        framework.application_productisation.release_gate
        tests/application_productisation/test_release_gate.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-runtime-commands-test
        framework.application_productisation.runtime_commands
        tests/application_productisation/test_runtime_commands.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-umbrella-test
        framework.application_productisation.umbrella
        tests/application_productisation/test_umbrella.c)
endif()
