/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_outbox.c
 *
 * PURPOSE:
 *   Verify durable outbox claiming, leases, successful delivery, bounded retries and dead-letter transition.
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
 * Exercise make entry and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiWorkbenchLayoutOutboxEntry make_entry(
    const char *entry_id,
    uint64_t sequence,
    uint64_t available_at_ms,
    uint32_t maximum_attempts)
{
    UmiWorkbenchLayoutOutboxEntry entry;
    (void)memset(&entry, 0, sizeof(entry));
    entry.structure_size = sizeof(entry);
    test_copy_text(entry.entry_id, sizeof(entry.entry_id), entry_id);
    test_copy_text(entry.topic, sizeof(entry.topic),
                   "umicom.layout.changed");
    test_copy_text(entry.aggregate_id,
                   sizeof(entry.aggregate_id), "layout.shared");
    test_copy_text(entry.correlation_id,
                   sizeof(entry.correlation_id), "correlation");
    entry.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING;
    entry.sequence = sequence;
    entry.created_at_ms = 1000U;
    entry.available_at_ms = available_at_ms;
    entry.maximum_attempts = maximum_attempts;
    test_copy_text(entry.payload_reference,
                   sizeof(entry.payload_reference),
                   "workbench-layout/layout-manifest/layout.shared");
    return entry;
}

/*
 * Exercise test outbox codec and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_outbox_codec(void)
{
    UmiWorkbenchLayoutOutboxEntry source = make_entry(
        "outbox-codec", 7U, 1500U, 4U);
    UmiWorkbenchLayoutOutboxEntry decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_outbox_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_outbox_decode(value, &decoded));
    TEST_REQUIRE(decoded.sequence == 7U, "sequence round trip");
    TEST_REQUIRE(decoded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING,
        "state round trip");
    TEST_REQUIRE(decoded.maximum_attempts == 4U,
                 "retry limit round trip");
    return 0;
}

/*
 * Exercise test claim and deliver and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_claim_and_deliver(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOutboxRepository repository;
    UmiWorkbenchLayoutOutboxEntry first = make_entry(
        "outbox-1", 1U, 1000U, 3U);
    UmiWorkbenchLayoutOutboxEntry second = make_entry(
        "outbox-2", 2U, 3000U, 3U);
    UmiWorkbenchLayoutOutboxEntry claimed;
    UmiWorkbenchLayoutOutboxEntry loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_outbox_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_outbox_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_outbox_save(
        &repository, &second));
    TEST_STATUS_OK(umi_workbench_layout_outbox_claim_available(
        &repository, 1500U, 500U, &claimed));
    TEST_REQUIRE(strcmp(claimed.entry_id, "outbox-1") == 0,
                 "earliest available entry claimed");
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_LEASED,
        "claim marks leased");
    TEST_REQUIRE(claimed.leased_until_ms == 2000U,
                 "claim lease deadline");
    TEST_STATUS_OK(umi_workbench_layout_outbox_mark_delivered(
        &repository, &claimed, 1600U));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED,
        "delivery state");
    TEST_STATUS_OK(umi_workbench_layout_outbox_load(
        &repository, NULL, "outbox-1", 1U, &loaded));
    TEST_REQUIRE(loaded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DELIVERED,
        "delivery persisted");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test retry and dead letter and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_retry_and_dead_letter(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOutboxRepository repository;
    UmiWorkbenchLayoutOutboxEntry entry = make_entry(
        "outbox-retry", 7U, 1000U, 2U);
    UmiWorkbenchLayoutOutboxEntry claimed;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_outbox_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_outbox_save(
        &repository, &entry));
    TEST_STATUS_OK(umi_workbench_layout_outbox_claim_available(
        &repository, 1000U, 100U, &claimed));
    TEST_STATUS_OK(umi_workbench_layout_outbox_mark_failed(
        &repository, &claimed, 1100U, 500U, "network unavailable"));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_RETRY_WAIT,
        "first failure waits for retry");
    TEST_REQUIRE(claimed.available_at_ms == 1600U,
                 "retry availability calculated");
    TEST_STATUS_OK(umi_workbench_layout_outbox_claim_available(
        &repository, 1600U, 100U, &claimed));
    TEST_STATUS_OK(umi_workbench_layout_outbox_mark_failed(
        &repository, &claimed, 1700U, 500U, "still unavailable"));
    TEST_REQUIRE(claimed.state ==
        UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_DEAD_LETTER,
        "retry limit leads to dead letter");
    TEST_REQUIRE(strcmp(claimed.last_error, "still unavailable") == 0,
                 "last failure retained");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test no available entry and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_no_available_entry(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutOutboxRepository repository;
    UmiWorkbenchLayoutOutboxEntry entry = make_entry(
        "outbox-future", 1U, 5000U, 3U);
    UmiWorkbenchLayoutOutboxEntry claimed;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_outbox_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_outbox_save(
        &repository, &entry));
    TEST_REQUIRE(umi_workbench_layout_outbox_claim_available(
        &repository, 1000U, 500U, &claimed) == UMI_STATUS_NOT_FOUND,
        "future entry not claimed");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_outbox_codec() == 0, "outbox codec");
    TEST_REQUIRE(test_claim_and_deliver() == 0, "claim deliver");
    TEST_REQUIRE(test_retry_and_dead_letter() == 0,
                 "retry dead letter");
    TEST_REQUIRE(test_no_available_entry() == 0,
                 "no available entry");
    return 0;
}
