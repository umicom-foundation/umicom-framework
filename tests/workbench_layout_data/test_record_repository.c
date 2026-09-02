/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_record_repository.c
 *
 * PURPOSE:
 *   Verify the bounded generic typed-record repository, predicates, pagination and Data Server namespace isolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

typedef struct FixtureRecord {
    uint32_t structure_size;
    char record_id[64];
    char aggregate_id[64];
    uint64_t sequence;
    bool enabled;
} FixtureRecord;

/*
 * Write fixture in its stable representation and report capacity or input failures to the
 * caller.
 */
static UmiStatus fixture_encode(
    const void *record_value,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    const FixtureRecord *record = (const FixtureRecord *)record_value;
    UmiWorkbenchLayoutDataFieldSet fields;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_layout_data_field_set_init(&fields);
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
        &fields, "record_id", record->record_id));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
        &fields, "aggregate_id", record->aggregate_id));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put_u64(
        &fields, "sequence", record->sequence));
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put_bool(
        &fields, "enabled", record->enabled));
    return umi_workbench_layout_data_value_encode(
        &fields, buffer, capacity, out_required);
}

/* Read fixture into validated module state and return a status when input cannot be used. */
static UmiStatus fixture_decode(const char *value, void *out_record_value)
{
    FixtureRecord *record = (FixtureRecord *)out_record_value;
    UmiWorkbenchLayoutDataFieldSet fields;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    record->structure_size = sizeof(*record);
    status = umi_workbench_layout_data_value_decode(value, &fields);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_copy_text(
            record->record_id, sizeof(record->record_id),
            umi_workbench_layout_data_field_set_get(&fields, "record_id"),
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_copy_text(
            record->aggregate_id, sizeof(record->aggregate_id),
            umi_workbench_layout_data_field_set_get(&fields, "aggregate_id"),
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "sequence", &record->sequence);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "enabled", &record->enabled);
    }
    return status;
}

/*
 * Exercise enabled predicate and return a clear result when the behaviour no longer
 * matches its contract.
 */
static UmiStatus enabled_predicate(
    const void *record_value,
    void *context,
    bool *out_matches)
{
    const FixtureRecord *record = (const FixtureRecord *)record_value;
    const bool expected = *(const bool *)context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || context == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches = record->enabled == expected;
    return UMI_STATUS_OK;
}

/*
 * Exercise fixture record and return a clear result when the behaviour no longer matches
 * its contract.
 */
static FixtureRecord fixture_record(
    const char *record_id,
    const char *aggregate_id,
    uint64_t sequence,
    bool enabled)
{
    FixtureRecord record;
    (void)memset(&record, 0, sizeof(record));
    record.structure_size = sizeof(record);
    test_copy_text(record.record_id, sizeof(record.record_id), record_id);
    test_copy_text(record.aggregate_id,
                   sizeof(record.aggregate_id), aggregate_id);
    record.sequence = sequence;
    record.enabled = enabled;
    return record;
}

/*
 * Exercise test repository lifecycle and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_repository_lifecycle(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataRecordRepository repository;
    FixtureRecord source = fixture_record(
        "cursor-a", "peer-a", 0U, true);
    FixtureRecord loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_data_record_repository_init(
        &repository, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        sizeof(FixtureRecord), fixture_encode, fixture_decode));
    TEST_STATUS_OK(umi_workbench_layout_data_record_repository_save(
        &repository, "peer-a", "cursor-a", 0U, &source));
    TEST_STATUS_OK(umi_workbench_layout_data_record_repository_load(
        &repository, "peer-a", "cursor-a", 0U, &loaded));
    TEST_REQUIRE(strcmp(loaded.record_id, "cursor-a") == 0,
                 "record identity restored");
    TEST_REQUIRE(loaded.enabled, "record state restored");
    TEST_STATUS_OK(umi_workbench_layout_data_record_repository_delete(
        &repository, "peer-a", "cursor-a", 0U));
    TEST_REQUIRE(umi_workbench_layout_data_record_repository_load(
        &repository, "peer-a", "cursor-a", 0U, &loaded) ==
        UMI_STATUS_NOT_FOUND, "deleted record absent");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test repository listing and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_repository_listing(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataRecordRepository repository;
    FixtureRecord records[5];
    FixtureRecord page_records[2];
    UmiWorkbenchLayoutDataRecordPage page;
    bool enabled = true;
    size_t index;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_data_record_repository_init(
        &repository, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        sizeof(FixtureRecord), fixture_encode, fixture_decode));
    records[0] = fixture_record("cursor-0", "peer-a", 0U, true);
    records[1] = fixture_record("cursor-1", "peer-a", 0U, false);
    records[2] = fixture_record("cursor-2", "peer-a", 0U, true);
    records[3] = fixture_record("cursor-3", "peer-b", 0U, true);
    records[4] = fixture_record("cursor-4", "peer-a", 0U, true);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 5U; ++index) {
        TEST_STATUS_OK(umi_workbench_layout_data_record_repository_save(
            &repository, records[index].aggregate_id,
            records[index].record_id, 0U, &records[index]));
    }
    (void)memset(&page, 0, sizeof(page));
    page.structure_size = sizeof(page);
    page.records = page_records;
    page.record_size = sizeof(page_records[0]);
    page.capacity = 2U;
    TEST_STATUS_OK(umi_workbench_layout_data_record_repository_list(
        &repository, "peer-a", enabled_predicate, &enabled, &page));
    TEST_REQUIRE(page.count == 2U, "bounded page filled");
    TEST_REQUIRE(page.total_available == 3U,
                 "all matching records counted");
    TEST_REQUIRE(page.truncated, "truncation reported");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < page.count; ++index) {
        TEST_REQUIRE(page_records[index].enabled,
                     "predicate applied to page");
        TEST_REQUIRE(strcmp(page_records[index].aggregate_id,
                            "peer-a") == 0,
                     "aggregate prefix applied");
    }
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test repository arguments and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_repository_arguments(void)
{
    UmiWorkbenchLayoutDataRecordRepository repository;
    UmiDataServer *server = test_create_data_server();
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(umi_workbench_layout_data_record_repository_init(
        &repository, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        0U, fixture_encode, fixture_decode) ==
        UMI_STATUS_INVALID_ARGUMENT, "zero record size rejected");
    TEST_REQUIRE(umi_workbench_layout_data_record_repository_init(
        &repository, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        sizeof(FixtureRecord), NULL, fixture_decode) ==
        UMI_STATUS_INVALID_ARGUMENT, "missing encoder rejected");
    TEST_REQUIRE(umi_workbench_layout_data_record_repository_init(
        &repository, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        sizeof(FixtureRecord), fixture_encode, NULL) ==
        UMI_STATUS_INVALID_ARGUMENT, "missing decoder rejected");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_repository_lifecycle() == 0,
                 "repository lifecycle");
    TEST_REQUIRE(test_repository_listing() == 0,
                 "repository listing");
    TEST_REQUIRE(test_repository_arguments() == 0,
                 "repository arguments");
    return 0;
}
