/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_launch_dispatch.c
 *
 * PURPOSE:
 *   Verify host-owned application opening, safe retries and reentrant callback
 *   protection without creating windows, processes or network connections.
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
#include <stdlib.h>
#include <string.h>

/* Report the precise failed expectation; main still releases the heap fixture. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return false; \
    } \
} while (0)

/* Reports and checkpoints contain bounded arrays. Keep them on the heap rather
 * than consuming the small default stack of a Windows test executable. */
typedef struct DispatchFixture {
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiApplicationLaunchSelection *selection;
    UmiApplicationLaunchSelection *other_selection;
    UmiApplicationLauncher *launcher;
    UmiApplicationLaunchDispatchReport report;
    UmiApplicationLaunchSelectionReport execute_report;
    UmiApplicationLaunchSelectionCheckpoint checkpoint;
    UmiApplicationLaunchChoice choice;
    UmiApplicationRuntimeRecord record;
    UmiApplicationLaunchSelectionSnapshot snapshot;
    char calls[UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS]
              [UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    size_t call_count;
    size_t launcher_calls;
    const char *failure_id;
    UmiStatus failure_status;
    bool check_reentrancy;
    bool reentrancy_verified;
    bool disable_bank;
} DispatchFixture;

/* The legacy launcher is supplied only to prove nested execution is refused. */
static UmiStatus unexpected_start(void *context,
                                  const UmiApplicationLaunchPlan *plan,
                                  uint64_t *out_token)
{
    DispatchFixture *fixture = (DispatchFixture *)context;
    (void)plan;
    (void)out_token;
    fixture->launcher_calls += 1U;
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Simulate an opener while recording the exact IDs it receives. No operating
 * system process API is called anywhere in this test. */
static UmiStatus record_request(const char *application_id, void *context)
{
    DispatchFixture *fixture = (DispatchFixture *)context;
    size_t length = strlen(application_id);
    if (fixture->call_count >= UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS ||
        length >= UMI_APPLICATION_RUNTIME_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(fixture->calls[fixture->call_count], application_id, length + 1U);
    fixture->call_count += 1U;

    /* A callback may read copies, but every mutation entry point must leave
     * the outer iterator and its report untouched until the request returns. */
    if (fixture->check_reentrancy) {
        const size_t reported_before = fixture->report.result_count;
        bool guarded = true;
        guarded = guarded && umi_application_launch_selection_set_selected(
            fixture->selection, application_id, false) == UMI_STATUS_BUSY;
        guarded = guarded && umi_application_launch_selection_clear(
            fixture->selection) == UMI_STATUS_BUSY;
        guarded = guarded && umi_application_launch_selection_select_all(
            fixture->selection) == UMI_STATUS_BUSY;
        guarded = guarded && umi_application_launch_selection_refresh(
            fixture->selection) == UMI_STATUS_BUSY;
        guarded = guarded && umi_application_launch_selection_checkpoint_restore(
            fixture->selection, &fixture->checkpoint) == UMI_STATUS_BUSY;
        guarded = guarded && umi_application_launch_selection_execute(
            fixture->selection, fixture->launcher,
            &fixture->execute_report) == UMI_STATUS_BUSY;
        guarded = guarded && umi_application_launch_selection_dispatch(
            fixture->selection, record_request, fixture,
            &fixture->report) == UMI_STATUS_BUSY;
        guarded = guarded && fixture->report.result_count == reported_before;
        guarded = guarded && umi_application_launch_selection_find(
            fixture->selection, application_id, &fixture->choice) == UMI_STATUS_OK;
        guarded = guarded && fixture->choice.selected;
        guarded = guarded && umi_application_launch_selection_snapshot(
            fixture->selection, &fixture->snapshot) == UMI_STATUS_OK;
        fixture->reentrancy_verified = guarded;
        if (!guarded) return UMI_STATUS_INTERNAL_ERROR;
    }
    /* Current policy must be checked again for a later selection, even when
     * the picker has not refreshed its cached eligibility flags yet. */
    if (fixture->disable_bank && strcmp(application_id, "org.umicom.studio") == 0) {
        UmiStatus status = umi_application_runtime_catalogue_set_presence(
            fixture->catalogue, "org.umicom.bank", false, true, true);
        if (status != UMI_STATUS_OK) return status;
    }
    if (fixture->failure_id != NULL &&
        strcmp(fixture->failure_id, application_id) == 0) {
        return fixture->failure_status;
    }
    return UMI_STATUS_OK;
}

/* Register a small known portfolio whose readiness gates are supplied by the
 * real Framework; the unknown ID deliberately has no registered experience. */
static bool prepare_fixture(DispatchFixture *fixture)
{
    static const char *const ids[] = {
        "org.umicom.desktop", "org.umicom.studio", "org.umicom.trader",
        "org.umicom.bank", "org.umicom.unknown"
    };
    UmiApplicationLauncherAdapter adapter = {0};
    size_t index;
    CHECK(umi_application_runtime_catalogue_create(&fixture->catalogue) == UMI_STATUS_OK);
    for (index = 0U; index < sizeof(ids) / sizeof(ids[0]); ++index) {
        UmiApplicationRuntimeRegistration registration = {0};
        registration.structure_size = sizeof(registration);
        registration.application_id = ids[index];
        registration.display_name = ids[index];
        registration.executable_name = "test-only-no-process";
        registration.working_directory = "";
        registration.icon_resource_id = "";
        registration.default_layout_id = "default";
        registration.taskbar_group = "applications";
        registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
        registration.maturity = UMI_APPLICATION_AVAILABLE;
        registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
        registration.installed = true;
        registration.compatible = true;
        registration.enabled = true;
        CHECK(umi_application_runtime_catalogue_register(
            fixture->catalogue, &registration) == UMI_STATUS_OK);
    }
    CHECK(umi_application_launch_selection_create_for_host(
        fixture->catalogue, NULL, &fixture->selection) == UMI_STATUS_OK);
    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = fixture;
    adapter.start = unexpected_start;
    CHECK(umi_application_launcher_create(fixture->catalogue, NULL,
        &adapter, &fixture->launcher) == UMI_STATUS_OK);
    return true;
}

/* Verify that changing the host affects only which application is omitted. */
static bool verify_host_selection(DispatchFixture *fixture)
{
    char oversized[UMI_APPLICATION_RUNTIME_ID_CAPACITY + 1U];
    CHECK(umi_application_launch_selection_create_for_host(fixture->catalogue,
        "org.umicom.studio", &fixture->other_selection) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_find(fixture->other_selection,
        "org.umicom.desktop", &fixture->choice) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_find(fixture->other_selection,
        "org.umicom.studio", &fixture->choice) == UMI_STATUS_NOT_FOUND);
    umi_application_launch_selection_destroy(fixture->other_selection);
    fixture->other_selection = NULL;
    CHECK(umi_application_launch_selection_create(fixture->catalogue,
        &fixture->other_selection) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_find(fixture->other_selection,
        "org.umicom.desktop", &fixture->choice) == UMI_STATUS_NOT_FOUND);
    /* Existing Desk callers retain the older refresh rule: an unavailable
     * choice is cleared rather than becoming a retained retry request. */
    CHECK(umi_application_launch_selection_set_selected(fixture->other_selection,
        "org.umicom.trader", true) == UMI_STATUS_OK);
    CHECK(umi_application_runtime_catalogue_set_presence(fixture->catalogue,
        "org.umicom.trader", false, true, true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_refresh(fixture->other_selection) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_find(fixture->other_selection,
        "org.umicom.trader", &fixture->choice) == UMI_STATUS_OK && !fixture->choice.selected);
    CHECK(umi_application_runtime_catalogue_set_presence(fixture->catalogue,
        "org.umicom.trader", true, true, true) == UMI_STATUS_OK);
    umi_application_launch_selection_destroy(fixture->other_selection);
    fixture->other_selection = NULL;
    CHECK(umi_application_launch_selection_create_for_host(fixture->catalogue,
        "org.umicom.missing", &fixture->other_selection) == UMI_STATUS_NOT_FOUND);
    CHECK(fixture->other_selection == NULL);
    (void)memset(oversized, 'a', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    CHECK(umi_application_launch_selection_create_for_host(fixture->catalogue,
        oversized, &fixture->other_selection) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(fixture->other_selection == NULL);
    CHECK(umi_application_launch_selection_create_for_host(fixture->catalogue,
        "", &fixture->other_selection) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_snapshot(fixture->other_selection,
        &fixture->snapshot) == UMI_STATUS_OK);
    CHECK(fixture->snapshot.choice_count == 5U);
    umi_application_launch_selection_destroy(fixture->other_selection);
    fixture->other_selection = NULL;
    return true;
}

/* Empty and malformed requests must never invoke the host or hide stale report
 * contents left by a previous operation. */
static bool verify_invalid_requests(DispatchFixture *fixture)
{
    (void)memset(&fixture->report, 0x55, sizeof(fixture->report));
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_INVALID_STATE);
    CHECK(fixture->report.result_count == 0U && fixture->report.accepted_count == 0U);
    CHECK(fixture->report.failed_count == 0U && fixture->call_count == 0U);
    CHECK(fixture->report.first_failure == UMI_STATUS_INVALID_STATE);
    CHECK(umi_application_launch_selection_dispatch(NULL, record_request,
        fixture, &fixture->report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_launch_selection_dispatch(fixture->selection, NULL,
        fixture, &fixture->report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "", true) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.missing", true) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.unknown", true) == UMI_STATUS_UNAVAILABLE);
    CHECK(fixture->call_count == 0U);
    return true;
}

/* A failure in the middle must not suppress the last request. Retry must
 * contain only the failed ID, and acceptance must not invent runtime state. */
static bool verify_partial_retry(DispatchFixture *fixture)
{
    static const char *const ids[] = {
        "org.umicom.studio", "org.umicom.trader", "org.umicom.bank"
    };
    size_t index;
    fixture->failure_id = "org.umicom.trader";
    fixture->failure_status = UMI_STATUS_IO_ERROR;
    for (index = 0U; index < sizeof(ids) / sizeof(ids[0]); ++index) {
        CHECK(umi_application_launch_selection_set_selected(
            fixture->selection, ids[index], true) == UMI_STATUS_OK);
    }
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_IO_ERROR);
    CHECK(fixture->report.result_count == 3U && fixture->call_count == 3U);
    CHECK(fixture->report.accepted_count == 2U && fixture->report.failed_count == 1U);
    for (index = 0U; index < sizeof(ids) / sizeof(ids[0]); ++index) {
        CHECK(strcmp(fixture->report.results[index].application_id, ids[index]) == 0);
        CHECK(strcmp(fixture->calls[index], ids[index]) == 0);
        CHECK(fixture->report.results[index].status ==
            (index == 1U ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
        CHECK(umi_application_launch_selection_find(fixture->selection,
            ids[index], &fixture->choice) == UMI_STATUS_OK);
        CHECK(fixture->choice.selected == (index == 1U));
        CHECK(umi_application_runtime_catalogue_find(fixture->catalogue,
            ids[index], &fixture->record) == UMI_STATUS_OK);
        CHECK(!fixture->record.running && fixture->record.process_token == 0U);
        CHECK(fixture->record.state == UMI_APPLICATION_RUNTIME_STOPPED);
    }
    fixture->failure_id = NULL;
    fixture->call_count = 0U;
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_OK);
    CHECK(fixture->report.result_count == 1U && fixture->report.accepted_count == 1U);
    CHECK(fixture->call_count == 1U && strcmp(fixture->calls[0], ids[1]) == 0);
    CHECK(umi_application_launch_selection_snapshot(fixture->selection,
        &fixture->snapshot) == UMI_STATUS_OK);
    CHECK(fixture->snapshot.selected_count == 0U && fixture->snapshot.running_count == 0U);
    return true;
}

/* Cancellation belongs to one host request, not to unrelated selected products. */
static bool verify_cancellation(DispatchFixture *fixture)
{
    fixture->failure_id = "org.umicom.studio";
    fixture->failure_status = UMI_STATUS_CANCELLED;
    fixture->call_count = 0U;
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.studio", true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.trader", true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_CANCELLED);
    CHECK(fixture->call_count == 2U && fixture->report.failed_count == 1U);
    CHECK(fixture->report.accepted_count == 1U);
    CHECK(umi_application_launch_selection_find(fixture->selection,
        "org.umicom.studio", &fixture->choice) == UMI_STATUS_OK && fixture->choice.selected);
    fixture->failure_id = NULL;
    CHECK(umi_application_launch_selection_clear(fixture->selection) == UMI_STATUS_OK);
    return true;
}

/* Policy changes made by one host request must block a later stale selection,
 * while leaving it checked so availability can be restored and retried. */
static bool verify_current_presence(DispatchFixture *fixture)
{
    fixture->disable_bank = true;
    fixture->call_count = 0U;
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.studio", true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.bank", true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_UNAVAILABLE);
    CHECK(fixture->call_count == 1U && fixture->report.result_count == 2U);
    CHECK(fixture->report.results[1].status == UMI_STATUS_UNAVAILABLE);
    CHECK(umi_application_launch_selection_find(fixture->selection,
        "org.umicom.bank", &fixture->choice) == UMI_STATUS_OK && fixture->choice.selected);
    CHECK(umi_application_launch_selection_refresh(fixture->selection) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_find(fixture->selection,
        "org.umicom.bank", &fixture->choice) == UMI_STATUS_OK && fixture->choice.selected);
    CHECK(!fixture->choice.eligible);
    /* Refresh must not silently discard a failed request, and keeping its
     * checkbox must not allow dispatch to bypass the unavailable gate. */
    fixture->call_count = 0U;
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_UNAVAILABLE);
    CHECK(fixture->call_count == 0U && fixture->report.failed_count == 1U);
    fixture->disable_bank = false;
    CHECK(umi_application_runtime_catalogue_set_presence(fixture->catalogue,
        "org.umicom.bank", true, true, true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_refresh(fixture->selection) == UMI_STATUS_OK);
    fixture->call_count = 0U;
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_OK);
    CHECK(fixture->call_count == 1U && strcmp(fixture->calls[0], "org.umicom.bank") == 0);
    return true;
}

/* Exercise every mutation guard from inside a real dispatch callback, then
 * prove the selection is unlocked again after that callback fails. */
static bool verify_reentrancy(DispatchFixture *fixture)
{
    fixture->check_reentrancy = true;
    fixture->failure_id = "org.umicom.studio";
    fixture->failure_status = UMI_STATUS_PERMISSION_DENIED;
    CHECK(umi_application_launch_selection_set_selected(fixture->selection,
        "org.umicom.studio", true) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_checkpoint_capture(fixture->selection,
        &fixture->checkpoint) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(fixture->reentrancy_verified && fixture->launcher_calls == 0U);
    fixture->check_reentrancy = false;
    fixture->failure_id = NULL;
    CHECK(umi_application_launch_selection_refresh(fixture->selection) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_dispatch(fixture->selection,
        record_request, fixture, &fixture->report) == UMI_STATUS_OK);
    CHECK(umi_application_launch_selection_clear(fixture->selection) == UMI_STATUS_OK);
    return true;
}

/* Run the native contract checks and release all owners even after a failure. */
int main(void)
{
    DispatchFixture *fixture = (DispatchFixture *)calloc(1U, sizeof(*fixture));
    bool passed;
    if (fixture == NULL) return 1;
    passed = prepare_fixture(fixture) && verify_host_selection(fixture) &&
        verify_invalid_requests(fixture) && verify_partial_retry(fixture) &&
        verify_cancellation(fixture) && verify_current_presence(fixture) &&
        verify_reentrancy(fixture);
    umi_application_launch_selection_destroy(fixture->other_selection);
    umi_application_launch_selection_destroy(fixture->selection);
    umi_application_launcher_destroy(fixture->launcher);
    umi_application_runtime_catalogue_destroy(fixture->catalogue);
    free(fixture);
    return passed ? 0 : 1;
}
