/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_native_discovery.c
 * PURPOSE: Verify atomic trusted GUI discovery and launch-time revalidation
 * with injected evidence only: no filesystem writes or child processes.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/native_discovery.h"
#include "umicom/application/portfolio.h"
#include "umicom/application/context_hub.h"
#include "umicom/desktop/desk_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define TEST_ROOT "C:/trusted/Umicom/bin"
#else
#define TEST_ROOT "/trusted/Umicom/bin"
#endif
#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

typedef struct Evidence {
    bool studio;
    bool bank;
    size_t calls;
    size_t fail_at;
    size_t starts;
    size_t activations;
    size_t stops;
    bool wrong_path;
} Evidence;

typedef struct Fixture {
    UmiApplicationRuntimeRecord before;
    UmiApplicationRuntimeRecord after;
    UmiApplicationRuntimeSnapshot snapshot;
    UmiApplicationRuntimeSnapshot unchanged;
    UmiApplicationNativeDiscoveryReport report;
    UmiApplicationLaunchPlan plan;
    UmiApplicationLaunchSelectionSnapshot selection;
    UmiDeskRuntimeSnapshot desk_before;
    UmiDeskRuntimeSnapshot desk_after;
    UmiDesktopTaskbarItem shell_item;
    UmiApplicationLauncherSnapshot launcher;
} Fixture;

/* The fake models installations, not execution, and rejects unexpected roots. */
static UmiStatus probe(const char *path, bool *present, void *context)
{
    Evidence *evidence = context;
    const size_t prefix_length = strlen(TEST_ROOT "/");
    ++evidence->calls;
    if (strncmp(path, TEST_ROOT "/", prefix_length) != 0 ||
        strstr(path, "-console") != NULL || strstr(path, "../") != NULL) {
        evidence->wrong_path = true;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (evidence->fail_at != 0U && evidence->calls == evidence->fail_at)
        return UMI_STATUS_PERMISSION_DENIED;
    *present = (strstr(path, "/umicom-studio-ide") != NULL && evidence->studio) ||
               (strstr(path, "/umicom-bank") != NULL && evidence->bank);
    return UMI_STATUS_OK;
}

/* Fake adapters only record governed requests; they never invoke an OS API. */
static UmiStatus start(void *context, const UmiApplicationLaunchPlan *plan, uint64_t *token)
{
    Evidence *evidence = context;
    (void)plan;
    ++evidence->starts;
    *token = 73U;
    return UMI_STATUS_OK;
}

/* Count requests to an existing process without presenting any window. */
static UmiStatus activate(void *context, const char *id, uint64_t token)
{
    Evidence *evidence = context;
    (void)id;
    if (token != 73U) return UMI_STATUS_INVALID_ARGUMENT;
    ++evidence->activations;
    return UMI_STATUS_OK;
}

/* Stop is a token assertion, not a process termination operation. */
static UmiStatus stop(void *context, const char *id, uint64_t token, uint32_t timeout)
{
    Evidence *evidence = context;
    (void)id;
    (void)timeout;
    if (token != 73U) return UMI_STATUS_INVALID_ARGUMENT;
    ++evidence->stops;
    return UMI_STATUS_OK;
}

/* Exercise presence, policy, process and stale-plan boundaries on one owner. */
int main(void)
{
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    UmiDeskRuntime *desk = NULL;
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *desktop = NULL;
    UmiDesktopShellModel *shell = NULL;
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiApplicationLauncher *launcher;
    UmiApplicationNativeDiscoveryConfig config = umi_application_native_discovery_config_default();
    UmiApplicationNativeDiscoveryConfig invalid;
    UmiDeskRuntimeConfig desk_config = umi_desk_runtime_config_default();
    UmiApplicationLauncherAdapter adapter = {0};
    Evidence evidence = {0};
    char temporary_root[sizeof(TEST_ROOT)];
    uint64_t baseline_revision;
    size_t baseline_calls;
    int result = 0;
    REQUIRE(fixture != NULL);
    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = &evidence;
    adapter.start = start;
    adapter.activate = activate;
    adapter.stop = stop;
    desk_config.launcher.executable_root = TEST_ROOT;
    REQUIRE(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    REQUIRE(umi_desktop_runtime_create(hub, &desktop) == UMI_STATUS_OK);
    REQUIRE(umi_desktop_runtime_seed(desktop) == UMI_STATUS_OK);
    REQUIRE(umi_desktop_shell_model_create(desktop, &shell) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_create(shell, &desk_config, &adapter, &desk) == UMI_STATUS_OK);
    catalogue = umi_desk_runtime_applications(desk);
    launcher = umi_desk_runtime_launcher(desk);
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 0U, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(fixture->report.skipped && evidence.calls == 0U);
    REQUIRE(umi_desk_runtime_admit_native_portfolio(desk) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_admit_native_portfolio(desk) == UMI_STATUS_INVALID_STATE);

    config.executable_root = TEST_ROOT;
    config.probe = probe;
    config.probe_context = &evidence;
    config.interval_ms = 1000U;
    invalid = config;
    invalid.executable_root = "relative/bin";
    REQUIRE(umi_application_native_discovery_config_validate(&invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid.executable_root = TEST_ROOT "/../escape";
    REQUIRE(umi_application_native_discovery_config_validate(&invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid.executable_root = "//remote/share";
    REQUIRE(umi_application_native_discovery_config_validate(&invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid = config;
    invalid.executable_suffix = ".cmd";
    REQUIRE(umi_application_native_discovery_config_validate(&invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid = config;
    invalid.executable_root = TEST_ROOT "/other";
    REQUIRE(umi_desk_runtime_configure_native_discovery(desk, &invalid) == UMI_STATUS_INVALID_STATE);
    memcpy(temporary_root, TEST_ROOT, sizeof(temporary_root));
    config.executable_root = temporary_root;
    REQUIRE(umi_desk_runtime_configure_native_discovery(desk, &config) == UMI_STATUS_OK);
    memset(temporary_root, 'x', sizeof(temporary_root) - 1U);
    config.executable_root = TEST_ROOT;
    evidence.studio = true;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 0U, false, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(fixture->report.scanned_count == 24U && fixture->report.installed_count == 1U);
    REQUIRE(!fixture->report.skipped && !evidence.wrong_path && evidence.starts == 0U);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.os", &fixture->after) == UMI_STATUS_OK);
    REQUIRE(strcmp(fixture->after.executable_name, "umicom-os-control-centre-gtk") == 0);
    REQUIRE(!fixture->after.installed);
    REQUIRE(umi_desk_runtime_snapshot(desk, &fixture->desk_before) == UMI_STATUS_OK);
    baseline_calls = evidence.calls;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 500U, false, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(fixture->report.skipped && evidence.calls == baseline_calls);
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 1000U, false, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(!fixture->report.skipped && fixture->report.changed_count == 0U);
    REQUIRE(umi_desk_runtime_snapshot(desk, &fixture->desk_after) == UMI_STATUS_OK);
    REQUIRE(fixture->desk_before.revision == fixture->desk_after.revision);
    REQUIRE(fixture->desk_before.applications.revision == fixture->desk_after.applications.revision);

    /* A failed scan cannot publish the earlier successful observations. */
    REQUIRE(umi_application_runtime_catalogue_snapshot(catalogue, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.studio", &fixture->before) == UMI_STATUS_OK);
    evidence.studio = false;
    evidence.bank = true;
    evidence.fail_at = evidence.calls + 24U;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 2000U, true, &fixture->report) == UMI_STATUS_PERMISSION_DENIED);
    REQUIRE(fixture->report.changed_count == 0U);
    REQUIRE(umi_application_runtime_catalogue_snapshot(catalogue, &fixture->unchanged) == UMI_STATUS_OK);
    REQUIRE(fixture->unchanged.revision == fixture->snapshot.revision);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.studio", &fixture->after) == UMI_STATUS_OK);
    REQUIRE(memcmp(&fixture->before, &fixture->after, sizeof(fixture->before)) == 0);
    evidence.fail_at = 0U;
    evidence.studio = true;
    evidence.bank = false;

    /* Activation without a process may exist in old catalogue clients. Its
     * active ID must clear when a staged absence makes that record inactive. */
    REQUIRE(umi_application_runtime_catalogue_activate(catalogue, "org.umicom.studio") == UMI_STATUS_OK);
    evidence.studio = false;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 2100U, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_snapshot(catalogue, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->snapshot.active_application_id[0] == '\0');
    evidence.studio = true;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 2200U, true, &fixture->report) == UMI_STATUS_OK);

    /* Policy and user pinning survive repeated file observations. */
    REQUIRE(umi_desk_runtime_set_application_presence(desk, "org.umicom.studio", true, false, false) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_pin(catalogue, "org.umicom.studio", true) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 1U, false, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(!fixture->report.skipped);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.studio", &fixture->after) == UMI_STATUS_OK);
    REQUIRE(!fixture->after.compatible && !fixture->after.enabled && fixture->after.pinned);
    REQUIRE(umi_desk_runtime_set_application_presence(desk, "org.umicom.studio", true, true, true) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_select_application(desk, "org.umicom.studio", true) == UMI_STATUS_OK);

    /* A selected, stopped app retains user intent through removal/reinstall. */
    evidence.studio = false;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 2500U, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_snapshot(umi_desk_runtime_launch_selection(desk), &fixture->selection) == UMI_STATUS_OK);
    REQUIRE(fixture->selection.selected_count == 1U && fixture->selection.running_count == 0U);
    evidence.studio = true;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 2600U, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(umi_application_launch_selection_snapshot(umi_desk_runtime_launch_selection(desk), &fixture->selection) == UMI_STATUS_OK);
    REQUIRE(fixture->selection.selected_count == 1U);

    /* Disk removal after prepare is caught before Start or Restart dispatch. */
    REQUIRE(umi_application_launcher_prepare(launcher, "org.umicom.studio", UMI_APPLICATION_LAUNCH_START, &fixture->plan) == UMI_STATUS_OK);
    evidence.studio = false;
    REQUIRE(umi_application_launcher_execute(launcher, &fixture->plan) == UMI_STATUS_UNAVAILABLE);
    REQUIRE(evidence.starts == 0U);
    fixture->plan.argument_count = UMI_APPLICATION_LAUNCH_MAX_ARGUMENTS + 1U;
    REQUIRE(umi_application_launcher_execute(launcher, &fixture->plan) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_application_launcher_snapshot(launcher, &fixture->launcher) == UMI_STATUS_OK);
    REQUIRE(fixture->launcher.last_plan.argument_count <= UMI_APPLICATION_LAUNCH_MAX_ARGUMENTS);
    REQUIRE(memchr(fixture->launcher.last_plan.application_id, '\0',
                   sizeof(fixture->launcher.last_plan.application_id)) != NULL);
    evidence.studio = true;
    REQUIRE(umi_application_launcher_request(launcher, "org.umicom.studio", UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_OK);
    REQUIRE(evidence.starts == 1U);
    REQUIRE(umi_application_launcher_prepare(launcher, "org.umicom.studio", UMI_APPLICATION_LAUNCH_RESTART, &fixture->plan) == UMI_STATUS_OK);
    evidence.studio = false;
    REQUIRE(umi_application_launcher_execute(launcher, &fixture->plan) == UMI_STATUS_UNAVAILABLE);
    REQUIRE(evidence.stops == 0U && evidence.starts == 1U);
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 3000U, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.studio", &fixture->after) == UMI_STATUS_OK);
    REQUIRE(!fixture->after.installed && fixture->after.running && fixture->after.active);
    REQUIRE(fixture->after.process_token == 73U && fixture->after.pinned);
    REQUIRE(umi_desktop_shell_model_find_application(shell, "org.umicom.studio", &fixture->shell_item) == UMI_STATUS_OK);
    REQUIRE(!fixture->shell_item.installed && fixture->shell_item.running &&
            fixture->shell_item.active && fixture->shell_item.pinned);
    REQUIRE(umi_application_launch_selection_snapshot(umi_desk_runtime_launch_selection(desk), &fixture->selection) == UMI_STATUS_OK);
    REQUIRE(fixture->selection.selected_count == 1U);
    REQUIRE(umi_desk_runtime_request_application(desk, "org.umicom.studio", UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE) == UMI_STATUS_OK);
    REQUIRE(evidence.activations == 1U);
    REQUIRE(umi_desk_runtime_request_application(desk, "org.umicom.studio", UMI_DESKTOP_APPLICATION_STRIP_STOP) == UMI_STATUS_OK);
    REQUIRE(evidence.stops == 1U);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.studio", &fixture->after) == UMI_STATUS_OK);
    REQUIRE(!fixture->after.running && fixture->after.process_token == 0U);
    REQUIRE(fixture->after.state == UMI_APPLICATION_RUNTIME_UNAVAILABLE);
    REQUIRE(umi_desktop_shell_model_find_application(shell, "org.umicom.studio", &fixture->shell_item) == UMI_STATUS_OK);
    REQUIRE(!fixture->shell_item.running && !fixture->shell_item.active);
    REQUIRE(umi_application_launcher_request(launcher, "org.umicom.studio", UMI_APPLICATION_LAUNCH_START) == UMI_STATUS_UNAVAILABLE);

    /* Reappearance changes eligibility, not the retained explicit selection. */
    evidence.studio = true;
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, 4000U, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_find(catalogue, "org.umicom.studio", &fixture->after) == UMI_STATUS_OK);
    REQUIRE(fixture->after.installed && !fixture->after.running && fixture->after.pinned);
    REQUIRE(umi_application_launch_selection_snapshot(umi_desk_runtime_launch_selection(desk), &fixture->selection) == UMI_STATUS_OK);
    REQUIRE(fixture->selection.selected_count == 1U);
    REQUIRE(umi_application_runtime_catalogue_snapshot(catalogue, &fixture->snapshot) == UMI_STATUS_OK);
    baseline_revision = fixture->snapshot.revision;
    baseline_calls = evidence.calls;
    REQUIRE(umi_desk_runtime_configure_native_discovery(desk, NULL) == UMI_STATUS_OK);
    REQUIRE(umi_desk_runtime_poll_native_discovery(desk, UINT64_MAX, true, &fixture->report) == UMI_STATUS_OK);
    REQUIRE(fixture->report.skipped && evidence.calls == baseline_calls);
    REQUIRE(umi_application_runtime_catalogue_snapshot(catalogue, &fixture->snapshot) == UMI_STATUS_OK);
    REQUIRE(fixture->snapshot.revision == baseline_revision);
cleanup:
    umi_desk_runtime_destroy(desk);
    umi_desktop_shell_model_destroy(shell);
    umi_desktop_runtime_destroy(desktop);
    umi_application_context_hub_destroy(hub);
    free(fixture);
    return result;
}
