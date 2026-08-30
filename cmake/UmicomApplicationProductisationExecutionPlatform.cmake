#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationProductisationExecutionPlatform.cmake
#
# PURPOSE:
#   Attach Framework-owned completion execution, governed Helix projection,
#   checkpoints, rollback and acceptance control to canonical productisation.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "Productisation execution requires canonical umicom_application")
endif()

set(UMICOM_PRODUCT_EXECUTION_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

target_sources(umicom_application PRIVATE
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/types.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/work_item.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/work_queue.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/dependency_graph.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/cancellation.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/policy.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/adapter.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/history.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/checkpoint.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/scheduler.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/evidence_bridge.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/metrics.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/rollback.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/acceptance.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/helix_bridge.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/executor.c"
    "${UMICOM_PRODUCT_EXECUTION_ROOT}/src/application/productisation/execution/runtime.c"
)
# Preserve the existing AI integration and add the canonical Helix action
# implementation called directly by the execution bridge. Both remain in the
# public closure for focused static-archive tests and SDK consumers.
if(TARGET Umicom::ai)
    target_link_libraries(umicom_application PUBLIC Umicom::ai)
endif()
if(TARGET Umicom::helix)
    target_link_libraries(umicom_application PUBLIC Umicom::helix)
endif()

if(BUILD_TESTING AND NOT TARGET umicom-application-productisation-execution-tests)
    add_executable(umicom-application-productisation-execution-tests
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_main.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_state_text.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_state_terminal.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_state_transition_valid.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_state_transition_invalid.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_work_item_init.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_work_item_retry.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_queue_from_plan.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_queue_bounds.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_dependency_add.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_dependency_duplicate.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_dependency_stage_barrier.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_dependency_satisfied.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_cancellation_request.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_cancellation_reset.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_policy_default.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_policy_invalid.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_policy_framework_first.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_policy_application_allowed.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_adapter_validate.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_adapter_external_missing.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_history_append.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_history_sequence.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_checkpoint_capture.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_checkpoint_latest.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_scheduler_framework_ready.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_scheduler_priority.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_metrics_collect.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_rollback_reverse.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_acceptance_pending.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_acceptance_success.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_helix_kind_acceptance.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_helix_kind_external.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_executor_success.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_executor_cancel.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_executor_dry_run.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_runtime_load.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_runtime_step.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_runtime_framework_then_app.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_runtime_checkpoint.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_runtime_cancel.c"
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution/test_runtime_snapshot.c"
)
    target_include_directories(umicom-application-productisation-execution-tests PRIVATE
        "${UMICOM_PRODUCT_EXECUTION_ROOT}/tests/application_productisation_execution")
    target_link_libraries(umicom-application-productisation-execution-tests PRIVATE
        Umicom::application)
    # These acceptance fixtures deliberately exercise full fixed-capacity plans,
    # queues and runtime ledgers as automatic values. Reserve a Windows test
    # stack comparable to Unix defaults instead of reducing production capacity.
    if(MINGW)
        target_link_options(umicom-application-productisation-execution-tests
            PRIVATE "-Wl,--stack,16777216")
    endif()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-application-productisation-execution-tests)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-application-productisation-execution-tests)
    endif()
    add_test(
        NAME framework.application_productisation.execution
        COMMAND umicom-application-productisation-execution-tests)
    set_tests_properties(framework.application_productisation.execution PROPERTIES
        LABELS "framework;application;productisation;execution;helix;acceptance")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(
            umicom-application-productisation-execution-tests)
    endif()
endif()
