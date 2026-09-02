/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_offline_queue.c
 *
 * PURPOSE:
 *   Verify durable offline operation replay, claim leases, retry scheduling, completion and terminal failure.
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
 * Exercise make operation and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiWorkbenchLayoutOfflineOperation make_operation(
    const char *operation_id,
    const char *layout_id,
    uint64_t available_at_ms,
    uint32_t maximum_attempts)
{
    UmiWorkbenchLayoutOfflineOperation operation;
    (void)memset(&operation, 0, sizeof(operation));
    operation.structure_size = sizeof(operation);
    test_copy_text(operation.operation_id,
                   sizeof(operation.operation_id), operation_id);
    test_copy_text(operation.layout_id,
                   sizeof(operation.layout_id), layout_id);
    test_copy_text(operation.actor_id,
                   sizeof(operation.actor_id), "developer");
    test_copy_text(operation.correlation_id,
                   sizeof(operation.correlation_id), "correlation");
    operation.change_kind = UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED;
    operation.state = UMI_WORKBENCH_LAYOUT_DATA_OPERATION_PENDING;
    operation.expected_revision = 1U;
    operation.created_at_ms = 1000U;
    operation.available_at_ms = available_at_ms;
    operation.maximum_attempts = maximum_attempts;
    test_copy_text(operation.payload_reference,
                   sizeof(operation.payload_reference),
                   "workbench-layout/layout-manifest/layout.offline");
    return operation;
}

/*
 * Exercise test offline codec and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_offline_codec(void)
{
    UmiWorkbenchLayoutOfflineOperation source = make_operation(
        "operation-codec", "layout.offline", 1500U, 4U);
    UmiWorkbenchLayoutOfflineOperation decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.change_kind ==
        UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
        "change kind round trip");
    TEST_REQUIRE(decoded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_PENDING,
        "operation state round trip");
    TEST_REQUIRE(decoded.maximum_attempts == 4U,
                 "attempt limit round trip");
    return 0;
}

/*
 * Exercise test claim and complete and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_claim_and_complete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOfflineQueueRepository repository;
    UmiWorkbenchLayoutOfflineOperation operation = make_operation(
        "operation-ready", "layout.offline", 1000U, 3U);
    UmiWorkbenchLayoutOfflineOperation claimed;
    UmiWorkbenchLayoutOfflineOperation loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_save(
        &repository, &operation));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_claim_available(
        &repository, 1000U, 500U, &claimed));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_CLAIMED,
        "operation claimed");
    TEST_REQUIRE(claimed.claimed_until_ms == 1500U,
        "claim lease recorded");
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_complete(
        &repository, &claimed));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED,
        "operation completed");
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_load(
        &repository, "layout.offline", "operation-ready", 0U, &loaded));
    TEST_REQUIRE(loaded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_COMPLETED,
        "completion persisted");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test retry then fail and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_retry_then_fail(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOfflineQueueRepository repository;
    UmiWorkbenchLayoutOfflineOperation operation = make_operation(
        "operation-retry", "layout.offline", 1000U, 2U);
    UmiWorkbenchLayoutOfflineOperation claimed;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_save(
        &repository, &operation));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_claim_available(
        &repository, 1000U, 100U, &claimed));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_retry(
        &repository, &claimed, 1100U, 500U, "offline"));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_RETRY_WAIT,
        "first failure schedules retry");
    TEST_REQUIRE(claimed.available_at_ms == 1600U,
        "retry availability recorded");
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_claim_available(
        &repository, 1600U, 100U, &claimed));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_retry(
        &repository, &claimed, 1700U, 500U, "still offline"));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OPERATION_FAILED,
        "maximum attempts produce failure");
    TEST_REQUIRE(strcmp(claimed.last_error, "still offline") == 0,
        "last error retained");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test future operation not claimed and return a clear result when the behaviour
 * no longer matches its contract.
 */
static int test_future_operation_not_claimed(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOfflineQueueRepository repository;
    UmiWorkbenchLayoutOfflineOperation operation = make_operation(
        "operation-future", "layout.offline", 5000U, 3U);
    UmiWorkbenchLayoutOfflineOperation claimed;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_offline_queue_save(
        &repository, &operation));
    TEST_REQUIRE(umi_workbench_layout_offline_queue_claim_available(
        &repository, 1000U, 500U, &claimed) == UMI_STATUS_NOT_FOUND,
        "future operation not claimed");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_offline_codec() == 0, "offline codec");
    TEST_REQUIRE(test_claim_and_complete() == 0,
                 "claim complete");
    TEST_REQUIRE(test_retry_then_fail() == 0,
                 "retry then fail");
    TEST_REQUIRE(test_future_operation_not_claimed() == 0,
                 "future operation");
    return 0;
}
