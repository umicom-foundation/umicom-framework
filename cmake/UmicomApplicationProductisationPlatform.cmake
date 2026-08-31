#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationProductisationPlatform.cmake
#
# PURPOSE:
#   Attach Framework-owned portfolio inventory, coverage evidence, shared-gap
#   analysis and completion planning to the canonical application target.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
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
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/surface_projection.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/adoption.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/adoption_registry.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/session.c"
    "${UMICOM_APPLICATION_PRODUCTISATION_ROOT}/src/application/productisation/workspace_guide.c"
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
    umicom_add_application_productisation_test(
        umicom-application-productisation-surface-projection-test
        framework.application_productisation.surface_projection
        tests/application_productisation/test_surface_projection.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-adoption-test
        framework.application_productisation.adoption
        tests/application_productisation/test_adoption.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-adoption-registry-test
        framework.application_productisation.adoption_registry
        tests/application_productisation/test_adoption_registry.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-session-test
        framework.application_productisation.session
        tests/application_productisation/test_session.c)
    umicom_add_application_productisation_test(
        umicom-application-productisation-workspace-guide-test
        framework.application_productisation.workspace_guide
        tests/application_productisation/test_workspace_guide.c)
endif()

# Completion plans remain canonical productisation data; this layer executes
# those plans through Framework-owned policy, Helix projection and evidence.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationProductisationExecutionPlatform.cmake")

# Product readiness now includes deterministic, toolkit-neutral layouts for
# every canonical application experience. Studio and Trader adopt this layer
# directly while the remaining suite applications inherit the same contract.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationSuiteLayoutPlatform.cmake")

# Canonical experiences, deterministic layouts and adoption snapshots now feed
# one production control plane used by every independently versioned product.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationProductionControlPlane.cmake")

# Product readiness is proven through executable user journeys that reference
# the canonical experience catalogue instead of duplicating panel, layout or
# feature declarations inside application repositories.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationJourneyPlatform.cmake")
