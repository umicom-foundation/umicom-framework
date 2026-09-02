/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_main.c
 *
 * PURPOSE:
 *   Run focused application experience runtime and projection tests.
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
 * Exercise test session and return a clear result when the behaviour no longer matches its
 * contract.
 */
int test_session(void);
/*
 * Exercise test panel state and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_panel_state(void);
/*
 * Exercise test layout session and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_layout_session(void);
/*
 * Exercise test activation plan and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_activation_plan(void);
/*
 * Exercise test readiness and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_readiness(void);
/*
 * Exercise test feature gate and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_feature_gate(void);
/*
 * Exercise test capability status and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_capability_status(void);
/*
 * Exercise test context binding and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_context_binding(void);
/*
 * Exercise test launch profile and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_launch_profile(void);
/*
 * Exercise test session snapshot and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_session_snapshot(void);
/*
 * Exercise test runtime health and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_runtime_health(void);
/*
 * Exercise test lifecycle evidence and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_lifecycle_evidence(void);
/*
 * Exercise test panel host and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_panel_host(void);
/*
 * Exercise test command surface and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_command_surface(void);
/*
 * Exercise test experience projection and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_experience_projection(void);
/*
 * Exercise test contract resolver and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_contract_resolver(void);
/*
 * Exercise test runtime catalogue and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_runtime_catalogue(void);
/*
 * Exercise test workspace catalogue and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_workspace_catalogue(void);
/*
 * Exercise test operation and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_operation(void);
/*
 * Exercise test module status and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_module_status(void);
/*
 * Exercise test workspace runtime and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_workspace_runtime(void);
/*
 * Exercise test ui summary and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_ui_summary(void);
/*
 * Exercise test ui readiness and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ui_readiness(void);
/*
 * Exercise test ui catalogues and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ui_catalogues(void);
/*
 * Exercise test ui health ownership and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ui_health_ownership(void);
/*
 * Exercise test ui session and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_ui_session(void);
/*
 * Exercise test application runtime workbench binding and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_application_runtime_workbench_binding(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_session() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_panel_state() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_layout_session() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_activation_plan() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_readiness() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_feature_gate() != 0) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (test_capability_status() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_context_binding() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_launch_profile() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_session_snapshot() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_runtime_health() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_lifecycle_evidence() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_panel_host() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_command_surface() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_experience_projection() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_contract_resolver() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_runtime_catalogue() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_workspace_catalogue() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_operation() != 0) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (test_module_status() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_workspace_runtime() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ui_summary() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ui_readiness() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ui_catalogues() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ui_health_ownership() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ui_session() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_application_runtime_workbench_binding() != 0) return 1;
    puts("application runtime tests passed");
    return 0;
}
