/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_desk_exit.c
 * PURPOSE: A stale or malformed exit report must not stop a current application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/desktop/desk_runtime.h"
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)
static UmiStatus Start(void *context, const UmiApplicationLaunchPlan *plan, uint64_t *token)
{
    uint64_t *next = context;
    (void)plan; *token = ++*next;
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiDeskRuntime *runtime = NULL;
    UmiDeskRuntimeConfig config = umi_desk_runtime_config_default();
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationRuntimeRegistration registration = {0};
    UmiApplicationRuntimeRecord before, after;
    UmiApplicationRuntimeSnapshot stateBefore, stateAfter;
    uint64_t next = 10U;
    char tooLong[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY + 1U];
    config.seed_framework_portfolio = false;
    config.launcher.executable_root = "."; config.launcher.executable_suffix = "";
    adapter.structure_size = sizeof(adapter); adapter.start = Start; adapter.adapter_context = &next;
    CHECK(umi_desk_runtime_create(NULL, &config, &adapter, &runtime) == UMI_STATUS_OK);
    registration.structure_size = sizeof(registration);
    registration.application_id = "org.umicom.lifecycle-test";
    registration.display_name = "Umicom lifecycle test";
    registration.executable_name = "never-launched";
    registration.working_directory = ""; registration.icon_resource_id = "umicom.icon.application.studio";
    registration.default_layout_id = "develop"; registration.taskbar_group = "development";
    registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    registration.maturity = UMI_APPLICATION_AVAILABLE;
    registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    registration.installed = true; registration.compatible = true; registration.enabled = true;
    CHECK(umi_desk_runtime_upsert_application(runtime, &registration) == UMI_STATUS_OK);
    CHECK(umi_desk_runtime_request_application(runtime, registration.application_id,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE) == UMI_STATUS_OK);
    UmiApplicationRuntimeCatalogue *catalogue = umi_desk_runtime_applications(runtime);
    CHECK(umi_application_runtime_catalogue_find(catalogue, registration.application_id, &before) == UMI_STATUS_OK);
    CHECK(before.process_token == 11U && before.running);
    CHECK(umi_application_runtime_catalogue_snapshot(catalogue, &stateBefore) == UMI_STATUS_OK);
    memset(tooLong, 'X', sizeof(tooLong)-1U); tooLong[sizeof(tooLong)-1U] = '\0';
    CHECK(UmiDeskRuntimeReconcileProcessExit(runtime, registration.application_id, 11U, 1, tooLong) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_application_runtime_catalogue_find(catalogue, registration.application_id, &after) == UMI_STATUS_OK);
    CHECK(memcmp(&before, &after, sizeof(before)) == 0);
    CHECK(umi_application_runtime_catalogue_snapshot(catalogue, &stateAfter) == UMI_STATUS_OK);
    CHECK(memcmp(&stateBefore, &stateAfter, sizeof(stateBefore)) == 0);
    CHECK(UmiDeskRuntimeReconcileProcessExit(runtime, registration.application_id, 11U, 0, "") == UMI_STATUS_OK);
    CHECK(umi_desk_runtime_request_application(runtime, registration.application_id,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE) == UMI_STATUS_OK);
    CHECK(umi_application_runtime_catalogue_find(catalogue, registration.application_id, &before) == UMI_STATUS_OK);
    CHECK(before.process_token == 12U && before.running);
    CHECK(UmiDeskRuntimeReconcileProcessExit(runtime, registration.application_id, 11U, 1, "old process") == UMI_STATUS_INVALID_STATE);
    CHECK(umi_application_runtime_catalogue_find(catalogue, registration.application_id, &after) == UMI_STATUS_OK);
    CHECK(memcmp(&before, &after, sizeof(before)) == 0);
    CHECK(UmiDeskRuntimeReconcileProcessExit(runtime, registration.application_id, 12U, 0, "") == UMI_STATUS_OK);
    CHECK(UmiDeskRuntimeReconcileProcessExit(runtime, registration.application_id, 12U, 0, "duplicate") == UMI_STATUS_INVALID_STATE);
    umi_desk_runtime_destroy(runtime);
    return EXIT_SUCCESS;
}
