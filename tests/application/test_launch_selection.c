/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_launch_selection.c
 *
 * PURPOSE:
 *   Verify that one selection can start several applications and report each
 *   result without bypassing the governed application launcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launch_selection.h"

#include <stdio.h>
#include <string.h>

#define REQUIRE(condition)                                                     \
    do {                                                                       \
        if (!(condition)) {                                                    \
            (void)fprintf(stderr, "[FAIL] %s:%d: %s\n",                       \
                          __FILE__, __LINE__, #condition);                     \
            return 1;                                                          \
        }                                                                      \
    } while (0)

typedef struct FakeProcesses {
    size_t starts;
    size_t activations;
} FakeProcesses;

/*
 * Exercise fake start and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus fake_start(void *context,
                            const UmiApplicationLaunchPlan *plan,
                            uint64_t *out_process_token)
{
    FakeProcesses *processes = (FakeProcesses *)context;
    (void)plan;
    processes->starts += 1U;
    *out_process_token = (uint64_t)(100U + processes->starts);
    return UMI_STATUS_OK;
}

/*
 * Exercise fake activate and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus fake_activate(void *context,
                               const char *application_id,
                               uint64_t process_token)
{
    FakeProcesses *processes = (FakeProcesses *)context;
    (void)application_id;
    REQUIRE(process_token != 0U);
    processes->activations += 1U;
    return UMI_STATUS_OK;
}

/*
 * Exercise registration and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiApplicationRuntimeRegistration registration(
    const char *application_id,
    const char *display_name,
    const char *executable_name)
{
    UmiApplicationRuntimeRegistration value = {0};
    value.structure_size = sizeof(value);
    value.application_id = application_id;
    value.display_name = display_name;
    value.executable_name = executable_name;
    value.working_directory = "";
    value.icon_resource_id = "";
    value.default_layout_id = "default";
    value.taskbar_group = "applications";
    value.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    value.maturity = UMI_APPLICATION_AVAILABLE;
    value.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    value.installed = true;
    value.compatible = true;
    value.enabled = true;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiApplicationLauncher *launcher = NULL;
    UmiApplicationLaunchSelection *selection = NULL;
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationLaunchSelectionSnapshot snapshot;
    UmiApplicationLaunchSelectionReport report;
    UmiApplicationRuntimeRegistration studio = registration(
        "org.umicom.studio", "Umicom Studio IDE", "umicom-studio-ide");
    UmiApplicationRuntimeRegistration trader = registration(
        "org.umicom.trader", "Umicom Trader", "umicom-trader");
    UmiApplicationRuntimeRegistration bank = registration(
        "org.umicom.bank", "Umicom Bank", "umicom-bank-console");
    FakeProcesses processes = {0};

    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = &processes;
    adapter.start = fake_start;
    adapter.activate = fake_activate;

    REQUIRE(umi_application_runtime_catalogue_create(&catalogue) ==
            UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &studio) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &trader) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &bank) == UMI_STATUS_OK);
    REQUIRE(umi_application_launcher_create(
                catalogue, NULL, &adapter, &launcher) == UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_create(
                catalogue, &selection) == UMI_STATUS_OK);

    REQUIRE(umi_application_launch_selection_set_selected(
                selection, "org.umicom.studio", true) == UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_set_selected(
                selection, "org.umicom.trader", true) == UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_snapshot(
                selection, &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.choice_count == 3U);
    REQUIRE(snapshot.selected_count == 2U);
    REQUIRE(umi_application_launch_selection_execute(
                selection, launcher, &report) == UMI_STATUS_OK);
    REQUIRE(report.result_count == 2U);
    REQUIRE(report.started_count == 2U);
    REQUIRE(report.failed_count == 0U);
    REQUIRE(processes.starts == 2U);

    /* Selecting a running application activates it instead of duplicating it. */
    REQUIRE(umi_application_launch_selection_clear(selection) ==
            UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_set_selected(
                selection, "org.umicom.studio", true) == UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_execute(
                selection, launcher, &report) == UMI_STATUS_OK);
    REQUIRE(report.activated_count == 1U);
    REQUIRE(processes.starts == 2U);
    REQUIRE(processes.activations == 1U);

    umi_application_launch_selection_destroy(selection);
    umi_application_launcher_destroy(launcher);
    umi_application_runtime_catalogue_destroy(catalogue);
    return 0;
}
