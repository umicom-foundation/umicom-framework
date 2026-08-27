#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationProductionControlPlane.cmake
#
# PURPOSE:
#   Attach the Framework-owned production bridge to the canonical application
#   target and register focused contract tests. Studio, Trader and the remaining
#   thin products consume these APIs without duplicating their logic.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_APPLICATION_PRODUCTION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "Application Production Control Plane requires umicom_application")
endif()

set(UMICOM_APPLICATION_PRODUCTION_MODULES
    identifier
    application_binding
    panel_binding
    layout_binding
    feature_binding
    command_binding
    registry
    capability_requirement
    capability_probe
    capability_readiness
    readiness_report
    workspace_checkpoint
    checkpoint_store
    workspace_recovery
    manifest_contract
    manifest_snapshot
    manifest_drift
    evidence_requirement
    evidence_record
    evidence_registry
    acceptance_rule
    acceptance_report
    lifecycle_gate
    launch_stage
    launch_plan
    runtime
    portfolio
    portfolio_report
    diagnostic_entry
    diagnostic_report
    command_catalogue
    control_plane
    audit
)

foreach(UMICOM_APPLICATION_PRODUCTION_MODULE
        IN LISTS UMICOM_APPLICATION_PRODUCTION_MODULES)
    target_sources(umicom_application PRIVATE
        "${UMICOM_APPLICATION_PRODUCTION_ROOT}/src/application/production/${UMICOM_APPLICATION_PRODUCTION_MODULE}.c")
endforeach()

if(UMICOM_BUILD_NATIVE_TOOL AND
   NOT TARGET umicom-application-production-audit)
    add_executable(umicom-application-production-audit
        "${UMICOM_APPLICATION_PRODUCTION_ROOT}/tools/application_production_audit_main.c")
    target_link_libraries(umicom-application-production-audit PRIVATE
        Umicom::application)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-application-production-audit)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-application-production-audit)
    endif()
    install(TARGETS umicom-application-production-audit
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(BUILD_TESTING)
    foreach(UMICOM_APPLICATION_PRODUCTION_MODULE
            IN LISTS UMICOM_APPLICATION_PRODUCTION_MODULES)
        string(REPLACE "_" "-"
            UMICOM_APPLICATION_PRODUCTION_TARGET_SUFFIX
            "${UMICOM_APPLICATION_PRODUCTION_MODULE}")
        set(UMICOM_APPLICATION_PRODUCTION_TEST_TARGET
            "umicom-application-production-${UMICOM_APPLICATION_PRODUCTION_TARGET_SUFFIX}-test")
        set(UMICOM_APPLICATION_PRODUCTION_TEST_NAME
            "framework.application_production.${UMICOM_APPLICATION_PRODUCTION_MODULE}")
        if(NOT TARGET "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}")
            add_executable("${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}"
                "${UMICOM_APPLICATION_PRODUCTION_ROOT}/tests/application_production/test_${UMICOM_APPLICATION_PRODUCTION_MODULE}.c")
            target_include_directories(
                "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}" PRIVATE
                "${UMICOM_APPLICATION_PRODUCTION_ROOT}/tests/application_production")
            target_link_libraries(
                "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}" PRIVATE
                Umicom::application)
            if(COMMAND umicom_apply_warnings)
                umicom_apply_warnings(
                    "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}")
            endif()
            if(COMMAND umicom_apply_sanitizers)
                umicom_apply_sanitizers(
                    "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}")
            endif()
            add_test(
                NAME "${UMICOM_APPLICATION_PRODUCTION_TEST_NAME}"
                COMMAND "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}")
            set_tests_properties(
                "${UMICOM_APPLICATION_PRODUCTION_TEST_NAME}" PROPERTIES
                LABELS
                "framework;application;production;layouts;acceptance")
            if(COMMAND umicom_register_validation_target)
                umicom_register_validation_target(
                    "${UMICOM_APPLICATION_PRODUCTION_TEST_TARGET}")
            endif()
        endif()
    endforeach()
endif()
