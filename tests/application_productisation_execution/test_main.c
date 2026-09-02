/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_main.c
 *
 * PURPOSE:
 *   Run the focused Framework productisation execution control-plane suite.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
/*
 * Exercise test state text and return a clear result when the behaviour no longer matches
 * its contract.
 */
void test_state_text(void);
/*
 * Exercise test state terminal and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_state_terminal(void);
/*
 * Check that test state transition satisfies its contract before another service relies on
 * it.
 */
void test_state_transition_valid(void);
/*
 * Exercise test state transition invalid and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_state_transition_invalid(void);
/*
 * Initialise test work item from caller-provided values so later operations receive a
 * known state.
 */
void test_work_item_init(void);
/*
 * Exercise test work item retry and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_work_item_retry(void);
/*
 * Exercise test queue from plan and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_queue_from_plan(void);
/*
 * Exercise test queue bounds and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_queue_bounds(void);
/* Add test dependency only after its inputs and available capacity have been checked. */
void test_dependency_add(void);
/*
 * Exercise test dependency duplicate and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_dependency_duplicate(void);
/*
 * Exercise test dependency stage barrier and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_dependency_stage_barrier(void);
/*
 * Exercise test dependency satisfied and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_dependency_satisfied(void);
/*
 * Exercise test cancellation request and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_cancellation_request(void);
/*
 * Release or reset state held by test cancellation so the same storage can be reused
 * safely.
 */
void test_cancellation_reset(void);
/*
 * Exercise test policy default and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_policy_default(void);
/*
 * Exercise test policy invalid and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_policy_invalid(void);
/*
 * Exercise test policy framework first and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_policy_framework_first(void);
/*
 * Exercise test policy application allowed and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_policy_application_allowed(void);
/* Check that test adapter satisfies its contract before another service relies on it. */
void test_adapter_validate(void);
/*
 * Exercise test adapter external missing and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_adapter_external_missing(void);
/* Add test history only after its inputs and available capacity have been checked. */
void test_history_append(void);
/*
 * Exercise test history sequence and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_history_sequence(void);
/*
 * Exercise test checkpoint capture and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_checkpoint_capture(void);
/*
 * Exercise test checkpoint latest and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_checkpoint_latest(void);
/*
 * Exercise test scheduler framework ready and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_scheduler_framework_ready(void);
/*
 * Exercise test scheduler priority and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_scheduler_priority(void);
/*
 * Exercise test metrics collect and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_metrics_collect(void);
/*
 * Exercise test rollback reverse and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_rollback_reverse(void);
/*
 * Exercise test acceptance pending and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_acceptance_pending(void);
/*
 * Exercise test acceptance success and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_acceptance_success(void);
/*
 * Exercise test helix kind acceptance and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_helix_kind_acceptance(void);
/*
 * Exercise test helix kind external and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_helix_kind_external(void);
/*
 * Exercise test executor success and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_executor_success(void);
/*
 * Exercise test executor cancel and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_executor_cancel(void);
/*
 * Perform test executor dry through the module contract so client applications do not
 * duplicate its policy.
 */
void test_executor_dry_run(void);
/*
 * Read test runtime into validated module state and return a status when input cannot be
 * used.
 */
void test_runtime_load(void);
/*
 * Exercise test runtime step and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_runtime_step(void);
/*
 * Exercise test runtime framework then app and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_runtime_framework_then_app(void);
/*
 * Exercise test runtime checkpoint and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_runtime_checkpoint(void);
/*
 * Exercise test runtime cancel and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_runtime_cancel(void);
/*
 * Exercise test runtime snapshot and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_runtime_snapshot(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    test_state_text();
    test_state_terminal();
    test_state_transition_valid();
    test_state_transition_invalid();
    test_work_item_init();
    test_work_item_retry();
    test_queue_from_plan();
    test_queue_bounds();
    test_dependency_add();
    test_dependency_duplicate();
    test_dependency_stage_barrier();
    test_dependency_satisfied();
    test_cancellation_request();
    test_cancellation_reset();
    test_policy_default();
    test_policy_invalid();
    test_policy_framework_first();
    test_policy_application_allowed();
    test_adapter_validate();
    test_adapter_external_missing();
    test_history_append();
    test_history_sequence();
    test_checkpoint_capture();
    test_checkpoint_latest();
    test_scheduler_framework_ready();
    test_scheduler_priority();
    test_metrics_collect();
    test_rollback_reverse();
    test_acceptance_pending();
    test_acceptance_success();
    test_helix_kind_acceptance();
    test_helix_kind_external();
    test_executor_success();
    test_executor_cancel();
    test_executor_dry_run();
    test_runtime_load();
    test_runtime_step();
    test_runtime_framework_then_app();
    test_runtime_checkpoint();
    test_runtime_cancel();
    test_runtime_snapshot();
    puts("application productisation execution tests passed");
    return 0;
}
