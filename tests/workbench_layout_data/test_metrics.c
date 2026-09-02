/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_metrics.c
 *
 * PURPOSE:
 *   Verify persistence and synchronisation metrics, error counters, byte totals, latency maxima, snapshots and reset.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

/*
 * Exercise test save load delete metrics and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_save_load_delete_metrics(void)
{
    UmiWorkbenchLayoutMetricsCollector collector;
    UmiWorkbenchLayoutDataMetrics snapshot;
    umi_workbench_layout_metrics_init(&collector);
    umi_workbench_layout_metrics_record_save(
        &collector, 100U, 20U, UMI_STATUS_OK);
    umi_workbench_layout_metrics_record_save(
        &collector, 50U, 40U, UMI_STATUS_IO_ERROR);
    umi_workbench_layout_metrics_record_load(
        &collector, 80U, 10U, UMI_STATUS_OK);
    umi_workbench_layout_metrics_record_delete(
        &collector, 5U, UMI_STATUS_OK);
    TEST_STATUS_OK(umi_workbench_layout_metrics_snapshot(
        &collector, &snapshot));
    TEST_REQUIRE(snapshot.save_count == 2U, "save count");
    TEST_REQUIRE(snapshot.load_count == 1U, "load count");
    TEST_REQUIRE(snapshot.delete_count == 1U, "delete count");
    TEST_REQUIRE(snapshot.bytes_written == 150U,
                 "written bytes counted");
    TEST_REQUIRE(snapshot.bytes_read == 80U,
                 "read bytes counted");
    TEST_REQUIRE(snapshot.maximum_operation_duration_us == 40U,
                 "maximum latency retained");
    return 0;
}

/*
 * Exercise test sync metrics and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_sync_metrics(void)
{
    UmiWorkbenchLayoutMetricsCollector collector;
    UmiWorkbenchLayoutDataMetrics snapshot;
    umi_workbench_layout_metrics_init(&collector);
    umi_workbench_layout_metrics_record_sync(
        &collector, true, 0U, 100U);
    umi_workbench_layout_metrics_record_sync(
        &collector, false, 2U, 250U);
    umi_workbench_layout_metrics_record_sync(
        &collector, true, 1U, 50U);
    TEST_STATUS_OK(umi_workbench_layout_metrics_snapshot(
        &collector, &snapshot));
    TEST_REQUIRE(snapshot.sync_plan_count == 3U,
                 "sync plan count");
    TEST_REQUIRE(snapshot.sync_success_count == 2U,
                 "sync success count");
    TEST_REQUIRE(snapshot.sync_failure_count == 1U,
                 "sync failure count");
    TEST_REQUIRE(snapshot.conflict_count == 3U,
                 "sync conflicts accumulated");
    return 0;
}

/*
 * Exercise test outbox offline metrics and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_outbox_offline_metrics(void)
{
    UmiWorkbenchLayoutMetricsCollector collector;
    UmiWorkbenchLayoutDataMetrics snapshot;
    umi_workbench_layout_metrics_init(&collector);
    umi_workbench_layout_metrics_record_outbox(
        &collector, true, false);
    umi_workbench_layout_metrics_record_outbox(
        &collector, false, true);
    umi_workbench_layout_metrics_record_offline_replay(
        &collector, true);
    umi_workbench_layout_metrics_record_offline_replay(
        &collector, false);
    TEST_STATUS_OK(umi_workbench_layout_metrics_snapshot(
        &collector, &snapshot));
    TEST_REQUIRE(snapshot.outbox_delivery_count == 1U,
                 "outbox delivery count");
    TEST_REQUIRE(snapshot.outbox_retry_count == 1U,
                 "outbox retry count");
    TEST_REQUIRE(snapshot.offline_replay_count == 2U,
                 "offline replay attempts counted");
    return 0;
}

/* Release or reset state held by test so the same storage can be reused safely. */
static int test_reset(void)
{
    UmiWorkbenchLayoutMetricsCollector collector;
    UmiWorkbenchLayoutDataMetrics snapshot;
    umi_workbench_layout_metrics_init(&collector);
    umi_workbench_layout_metrics_record_save(
        &collector, 100U, 20U, UMI_STATUS_OK);
    umi_workbench_layout_metrics_reset(&collector);
    TEST_STATUS_OK(umi_workbench_layout_metrics_snapshot(
        &collector, &snapshot));
    TEST_REQUIRE(snapshot.save_count == 0U, "save count reset");
    TEST_REQUIRE(snapshot.bytes_written == 0U, "bytes reset");
    TEST_REQUIRE(snapshot.revision > 0U, "reset revision valid");
    return 0;
}

/*
 * Exercise test invalid snapshot and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_invalid_snapshot(void)
{
    UmiWorkbenchLayoutMetricsCollector collector;
    umi_workbench_layout_metrics_init(&collector);
    TEST_REQUIRE(umi_workbench_layout_metrics_snapshot(
        &collector, NULL) == UMI_STATUS_INVALID_ARGUMENT,
        "null output rejected");
    TEST_REQUIRE(umi_workbench_layout_metrics_snapshot(
        NULL, &collector.snapshot) == UMI_STATUS_INVALID_ARGUMENT,
        "null collector rejected");
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_save_load_delete_metrics() == 0,
                 "save load delete metrics");
    TEST_REQUIRE(test_sync_metrics() == 0, "sync metrics");
    TEST_REQUIRE(test_outbox_offline_metrics() == 0,
                 "outbox offline metrics");
    TEST_REQUIRE(test_reset() == 0, "metrics reset");
    TEST_REQUIRE(test_invalid_snapshot() == 0,
                 "invalid metrics snapshot");
    return 0;
}
