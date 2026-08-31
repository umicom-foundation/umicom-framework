/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_desk_runtime.c
 *
 * PURPOSE:
 *   Verify the unified Desk runtime without requiring a graphical frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/desk_runtime.h"

#include <stdio.h>
#include <string.h>

#define REQUIRE(condition)                                                     \
    do {                                                                       \
        if (!(condition)) {                                                    \
            (void)fprintf(stderr, "[FAIL] %s:%d: %s\n",                       \
                          __FILE__, __LINE__, #condition);                      \
            return 1;                                                          \
        }                                                                      \
    } while (0)

typedef struct FakeLaunch {
    uint64_t next_process;
} FakeLaunch;

static UmiStatus start_application(
    void *context,
    const UmiApplicationLaunchPlan *plan,
    uint64_t *out_process_token)
{
    FakeLaunch *launch = (FakeLaunch *)context;
    (void)plan;
    launch->next_process += 1U;
    *out_process_token = launch->next_process;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiDeskRuntime *runtime = NULL;
    UmiDeskRuntimeConfig config = umi_desk_runtime_config_default();
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationRuntimeRegistration registration = {0};
    UmiApplicationRuntimeRegistration trader = {0};
    UmiDeskRuntimeSnapshot snapshot;
    UmiApplicationLaunchSelectionReport launch_report;
    FakeLaunch launch = {100U};

    config.seed_framework_portfolio = false;
    config.launcher.executable_root = ".";
    config.launcher.executable_suffix = "";

    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = &launch;
    adapter.start = start_application;

    REQUIRE(umi_desk_runtime_create(
                NULL, &config, &adapter, &runtime) == UMI_STATUS_OK);

    registration.structure_size = sizeof(registration);
    registration.application_id = "org.umicom.studio";
    registration.display_name = "Umicom Studio IDE";
    registration.executable_name = "umicom-studio-ide";
    registration.working_directory = "";
    registration.icon_resource_id = "umicom.icon.application.studio";
    registration.default_layout_id = "develop";
    registration.taskbar_group = "development";
    registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    registration.maturity = UMI_APPLICATION_AVAILABLE;
    registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    registration.installed = true;
    registration.compatible = true;
    registration.enabled = true;
    registration.pinned = true;

    REQUIRE(umi_desk_runtime_upsert_application(
                runtime, &registration) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_request_application(
                runtime, "org.umicom.studio",
                UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE) ==
            UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_snapshot(runtime, &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.applications.running_application_count == 1U);
    REQUIRE(snapshot.strip.running_count == 1U);
    REQUIRE(strcmp(snapshot.strip.active_application_id,
                   "org.umicom.studio") == 0);
    REQUIRE(!snapshot.has_shell);

    trader = registration;
    trader.application_id = "org.umicom.trader";
    trader.display_name = "Umicom Trader";
    trader.executable_name = "umicom-trader";
    trader.icon_resource_id = "umicom.icon.application.trader";
    trader.default_layout_id = "trading";
    trader.taskbar_group = "trading";
    trader.family = UMI_APPLICATION_FAMILY_FINANCE;
    REQUIRE(umi_desk_runtime_upsert_application(
                runtime, &trader) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_select_application(
                runtime, "org.umicom.studio", true) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_select_application(
                runtime, "org.umicom.trader", true) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_launch_selected_applications(
                runtime, &launch_report) == UMI_STATUS_OK);
    REQUIRE(launch_report.result_count == 2U);
    REQUIRE(launch_report.started_count == 1U);
    REQUIRE(launch_report.activated_count == 1U);

    REQUIRE(umi_desk_runtime_reconcile_application_exit(
                runtime, "org.umicom.studio", 0, "") == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_reconcile_application_exit(
                runtime, "org.umicom.trader", 0, "") == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_snapshot(runtime, &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.applications.running_application_count == 0U);

    umi_desk_runtime_destroy(runtime);
    return 0;
}
