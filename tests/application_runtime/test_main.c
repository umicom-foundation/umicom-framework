/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_main.c
 *
 * PURPOSE:
 *   Run focused application experience runtime and projection tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>

int test_session(void);
int test_panel_state(void);
int test_layout_session(void);
int test_activation_plan(void);
int test_readiness(void);
int test_feature_gate(void);
int test_capability_status(void);
int test_context_binding(void);
int test_launch_profile(void);
int test_session_snapshot(void);
int test_runtime_health(void);
int test_lifecycle_evidence(void);
int test_panel_host(void);
int test_command_surface(void);
int test_experience_projection(void);
int test_contract_resolver(void);
int test_runtime_catalogue(void);
int test_workspace_catalogue(void);
int test_operation(void);
int test_module_status(void);
int test_workspace_runtime(void);
int test_ui_summary(void);
int test_ui_readiness(void);
int test_ui_catalogues(void);
int test_ui_health_ownership(void);
int test_ui_session(void);

int main(void)
{
    if (test_session() != 0) return 1;
    if (test_panel_state() != 0) return 1;
    if (test_layout_session() != 0) return 1;
    if (test_activation_plan() != 0) return 1;
    if (test_readiness() != 0) return 1;
    if (test_feature_gate() != 0) return 1;
    if (test_capability_status() != 0) return 1;
    if (test_context_binding() != 0) return 1;
    if (test_launch_profile() != 0) return 1;
    if (test_session_snapshot() != 0) return 1;
    if (test_runtime_health() != 0) return 1;
    if (test_lifecycle_evidence() != 0) return 1;
    if (test_panel_host() != 0) return 1;
    if (test_command_surface() != 0) return 1;
    if (test_experience_projection() != 0) return 1;
    if (test_contract_resolver() != 0) return 1;
    if (test_runtime_catalogue() != 0) return 1;
    if (test_workspace_catalogue() != 0) return 1;
    if (test_operation() != 0) return 1;
    if (test_module_status() != 0) return 1;
    if (test_workspace_runtime() != 0) return 1;
    if (test_ui_summary() != 0) return 1;
    if (test_ui_readiness() != 0) return 1;
    if (test_ui_catalogues() != 0) return 1;
    if (test_ui_health_ownership() != 0) return 1;
    if (test_ui_session() != 0) return 1;
    puts("application runtime tests passed");
    return 0;
}
