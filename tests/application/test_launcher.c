/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_launcher.c
 *
 * PURPOSE:
 *   Verify immutable launch planning and adapter-governed lifecycle changes.
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

#define REQUIRE(condition)                                                     \
    do {                                                                       \
        if (!(condition)) {                                                    \
            (void)fprintf(stderr, "[FAIL] %s:%d: %s\n",                       \
                          __FILE__, __LINE__, #condition);                      \
            return 1;                                                          \
        }                                                                      \
    } while (0)

typedef struct FakeAdapter {
    size_t starts;
    size_t activations;
    size_t stops;
    UmiApplicationLaunchPlan last_plan;
} FakeAdapter;

/*
 * Exercise fake start and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus fake_start(
    void *context,
    const UmiApplicationLaunchPlan *plan,
    uint64_t *out_process_token)
{
    FakeAdapter *adapter = (FakeAdapter *)context;
    adapter->starts += 1U;
    adapter->last_plan = *plan;
    *out_process_token = 99U;
    return UMI_STATUS_OK;
}

/*
 * Exercise fake activate and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus fake_activate(
    void *context,
    const char *application_id,
    uint64_t process_token)
{
    FakeAdapter *adapter = (FakeAdapter *)context;
    (void)application_id;
    REQUIRE(process_token == 99U);
    adapter->activations += 1U;
    return UMI_STATUS_OK;
}

/*
 * Exercise fake stop and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus fake_stop(
    void *context,
    const char *application_id,
    uint64_t process_token,
    uint32_t timeout_ms)
{
    FakeAdapter *adapter = (FakeAdapter *)context;
    (void)application_id;
    REQUIRE(process_token == 99U);
    REQUIRE(timeout_ms == 2500U);
    adapter->stops += 1U;
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiApplicationLauncher *launcher = NULL;
    UmiApplicationRuntimeRegistration registration = {0};
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationLaunchConfig config =
        umi_application_launch_config_default();
    UmiApplicationLaunchPlan plan;
    UmiApplicationRuntimeRecord record;
    UmiApplicationLauncherSnapshot snapshot;
    FakeAdapter fake = {0};

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

    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = &fake;
    adapter.start = fake_start;
    adapter.activate = fake_activate;
    adapter.stop = fake_stop;

    config.executable_root = "C:/umicom/bin";
    config.default_working_directory = "C:/umicom";
    config.executable_suffix = ".exe";
    config.graceful_stop_timeout_ms = 2500U;

    REQUIRE(umi_application_runtime_catalogue_create(&catalogue) ==
            UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &registration) == UMI_STATUS_OK);
    REQUIRE(umi_application_launcher_create(
                catalogue, &config, &adapter, &launcher) == UMI_STATUS_OK);

    REQUIRE(umi_application_launcher_prepare(
                launcher, "org.umicom.studio",
                UMI_APPLICATION_LAUNCH_START, &plan) == UMI_STATUS_OK);
    REQUIRE(strcmp(plan.executable_path,
                   "C:/umicom/bin/umicom-studio-ide.exe") == 0);
    REQUIRE(umi_application_launcher_execute(
                launcher, &plan) == UMI_STATUS_OK);
    REQUIRE(fake.starts == 1U);
    REQUIRE(umi_application_runtime_catalogue_find(
                catalogue, "org.umicom.studio", &record) == UMI_STATUS_OK);
    REQUIRE(record.running);
    REQUIRE(record.active);
    REQUIRE(record.process_token == 99U);

    REQUIRE(umi_application_launcher_request(
                launcher, "org.umicom.studio",
                UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_OK);
    REQUIRE(fake.activations == 1U);

    REQUIRE(umi_application_launcher_request(
                launcher, "org.umicom.studio",
                UMI_APPLICATION_LAUNCH_STOP) == UMI_STATUS_OK);
    REQUIRE(fake.stops == 1U);

    REQUIRE(umi_application_launcher_snapshot(
                launcher, &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.request_count == 3U);
    REQUIRE(snapshot.failed_request_count == 0U);

    umi_application_launcher_destroy(launcher);
    umi_application_runtime_catalogue_destroy(catalogue);
    return 0;
}
