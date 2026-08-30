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
void test_state_text(void);
void test_state_terminal(void);
void test_state_transition_valid(void);
void test_state_transition_invalid(void);
void test_work_item_init(void);
void test_work_item_retry(void);
void test_queue_from_plan(void);
void test_queue_bounds(void);
void test_dependency_add(void);
void test_dependency_duplicate(void);
void test_dependency_stage_barrier(void);
void test_dependency_satisfied(void);
void test_cancellation_request(void);
void test_cancellation_reset(void);
void test_policy_default(void);
void test_policy_invalid(void);
void test_policy_framework_first(void);
void test_policy_application_allowed(void);
void test_adapter_validate(void);
void test_adapter_external_missing(void);
void test_history_append(void);
void test_history_sequence(void);
void test_checkpoint_capture(void);
void test_checkpoint_latest(void);
void test_scheduler_framework_ready(void);
void test_scheduler_priority(void);
void test_metrics_collect(void);
void test_rollback_reverse(void);
void test_acceptance_pending(void);
void test_acceptance_success(void);
void test_helix_kind_acceptance(void);
void test_helix_kind_external(void);
void test_executor_success(void);
void test_executor_cancel(void);
void test_executor_dry_run(void);
void test_runtime_load(void);
void test_runtime_step(void);
void test_runtime_framework_then_app(void);
void test_runtime_checkpoint(void);
void test_runtime_cancel(void);
void test_runtime_snapshot(void);

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
