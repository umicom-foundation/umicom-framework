/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_deferred_launch.c
 *
 * PURPOSE:
 *   Verify asynchronous launch admission never claims native creation or focus before evidence arrives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/application/launcher.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct Fixture { unsigned starts, activations; UmiStatus status; } Fixture;
static UmiStatus Start(void *context, const UmiApplicationLaunchPlan *plan, uint64_t *token)
{
    Fixture *fixture = context; (void)plan;
    ++fixture->starts; *token = 41U; return fixture->status;
}
static UmiStatus Activate(void *context, const char *id, uint64_t token)
{
    Fixture *fixture = context; (void)id; (void)token;
    ++fixture->activations; return UMI_STATUS_OK;
}
int main(void)
{
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiApplicationLauncher *launcher = NULL;
    Fixture fixture = {0};
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationRuntimeRegistration entry = {0};
    UmiApplicationRuntimeRecord record;
    CHECK(umi_application_runtime_catalogue_create(&catalogue) == UMI_STATUS_OK);
    entry.structure_size = sizeof(entry); entry.application_id = "org.umicom.studio";
    entry.display_name = "Studio fixture"; entry.executable_name = "inert-fixture";
    entry.working_directory = ""; entry.icon_resource_id = "umicom.icon.application.studio";
    entry.default_layout_id = "develop"; entry.taskbar_group = "development";
    entry.family = UMI_APPLICATION_FAMILY_DEVELOPMENT; entry.maturity = UMI_APPLICATION_AVAILABLE;
    entry.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    entry.installed = true; entry.enabled = true; entry.compatible = true;
    CHECK(umi_application_runtime_catalogue_upsert(catalogue, &entry) == UMI_STATUS_OK);
    adapter.structure_size = sizeof(adapter); adapter.adapter_context = &fixture;
    adapter.start = Start; adapter.activate = Activate;
    CHECK(umi_application_launcher_create(catalogue, NULL, &adapter, &launcher) == UMI_STATUS_OK);
    CHECK(UmiApplicationLauncherSetDeferredStart(NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiApplicationLauncherSetDeferredStart(launcher, true) == UMI_STATUS_OK);
    CHECK(umi_application_launcher_request(launcher, entry.application_id, UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_OK);
    CHECK(umi_application_runtime_catalogue_find(catalogue, entry.application_id, &record) == UMI_STATUS_OK);
    CHECK(record.state == UMI_APPLICATION_RUNTIME_STARTING && record.running && !record.active && record.process_token == 41U);
    CHECK(UmiApplicationLauncherSetDeferredStart(launcher, false) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_application_launcher_request(launcher, entry.application_id, UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_BUSY);
    CHECK(fixture.starts == 1U && fixture.activations == 0U);
    CHECK(umi_application_runtime_catalogue_set_state(catalogue, entry.application_id,
        UMI_APPLICATION_RUNTIME_RUNNING, "Native creation observed") == UMI_STATUS_OK);
    UmiApplicationLaunchPlan plan;
    CHECK(umi_application_launcher_prepare(launcher, entry.application_id, UMI_APPLICATION_LAUNCH_ACTIVATE, &plan) == UMI_STATUS_OK);
    CHECK(umi_application_runtime_catalogue_set_state(catalogue, entry.application_id,
        UMI_APPLICATION_RUNTIME_STOPPING, "Stopping") == UMI_STATUS_OK);
    CHECK(umi_application_launcher_execute(launcher, &plan) == UMI_STATUS_BUSY && fixture.activations == 0U);
    CHECK(umi_application_launcher_reconcile_exit(launcher, entry.application_id, 7, "Startup failed") == UMI_STATUS_OK);
    CHECK(umi_application_runtime_catalogue_find(catalogue, entry.application_id, &record) == UMI_STATUS_OK);
    CHECK(!record.running && !record.active && record.last_exit_code == 7);
    fixture.status = UMI_STATUS_UNAVAILABLE;
    CHECK(umi_application_launcher_request(launcher, entry.application_id, UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_UNAVAILABLE);
    CHECK(umi_application_runtime_catalogue_find(catalogue, entry.application_id, &record) == UMI_STATUS_OK);
    CHECK(record.state == UMI_APPLICATION_RUNTIME_FAILED && !record.running);
    umi_application_launcher_destroy(launcher);
    umi_application_runtime_catalogue_destroy(catalogue);
    return 0;
}
