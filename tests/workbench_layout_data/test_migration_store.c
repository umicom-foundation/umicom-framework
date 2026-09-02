/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_migration_store.c
 *
 * PURPOSE:
 *   Verify durable schema-migration records, state evidence, deterministic encoding and bounded history listing.
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
 * Exercise make migration record and return a clear result when the behaviour no longer
 * matches its contract.
 */
static UmiWorkbenchLayoutMigrationRecord make_migration_record(
    const char *migration_id,
    uint32_t from_version,
    uint32_t to_version,
    UmiWorkbenchLayoutDataMigrationState state)
{
    UmiWorkbenchLayoutMigrationRecord record;
    (void)memset(&record, 0, sizeof(record));
    record.structure_size = sizeof(record);
    record.descriptor.structure_size = sizeof(record.descriptor);
    test_copy_text(record.descriptor.migration_id,
                   sizeof(record.descriptor.migration_id), migration_id);
    test_copy_text(record.descriptor.name,
                   sizeof(record.descriptor.name), "Layout schema migration");
    record.descriptor.from_version = from_version;
    record.descriptor.to_version = to_version;
    record.descriptor.checksum = 1000U + to_version;
    record.descriptor.reversible = true;
    record.state = state;
    test_copy_text(record.applied_by,
                   sizeof(record.applied_by), "developer");
    record.started_at_ms = 1000U;
    record.completed_at_ms = state ==
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_APPLIED ? 2000U : 0U;
    return record;
}

/*
 * Exercise test migration record codec and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_migration_record_codec(void)
{
    UmiWorkbenchLayoutMigrationRecord source = make_migration_record(
        "layout-schema-1-2", 1U, 2U,
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_APPLIED);
    UmiWorkbenchLayoutMigrationRecord decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_migration_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_migration_store_decode(
        value, &decoded));
    TEST_REQUIRE(strcmp(decoded.descriptor.migration_id,
        "layout-schema-1-2") == 0, "migration id round trip");
    TEST_REQUIRE(decoded.descriptor.from_version == 1U,
                 "from version round trip");
    TEST_REQUIRE(decoded.descriptor.to_version == 2U,
                 "to version round trip");
    TEST_REQUIRE(decoded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_APPLIED,
        "migration state round trip");
    return 0;
}

/*
 * Exercise test migration repository and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_migration_repository(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutMigrationStoreRepository repository;
    UmiWorkbenchLayoutMigrationRecord first = make_migration_record(
        "migration-1", 0U, 1U,
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_APPLIED);
    UmiWorkbenchLayoutMigrationRecord second = make_migration_record(
        "migration-2", 1U, 2U,
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_PENDING);
    UmiWorkbenchLayoutMigrationRecord loaded;
    UmiWorkbenchLayoutMigrationRecord records[4];
    size_t count = 0U;
    size_t total = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_migration_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_migration_store_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_migration_store_save(
        &repository, &second));
    TEST_STATUS_OK(umi_workbench_layout_migration_store_load(
        &repository, NULL, "migration-2", 0U, &loaded));
    TEST_REQUIRE(loaded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_PENDING,
        "migration loaded");
    TEST_STATUS_OK(umi_workbench_layout_migration_store_list(
        &repository, NULL, NULL, NULL,
        records, 4U, &count, &total));
    TEST_REQUIRE(count == 2U && total == 2U,
                 "migration history listed");
    TEST_STATUS_OK(umi_workbench_layout_migration_store_delete(
        &repository, NULL, "migration-2", 0U));
    TEST_REQUIRE(umi_workbench_layout_migration_store_load(
        &repository, NULL, "migration-2", 0U, &loaded) ==
        UMI_STATUS_NOT_FOUND, "migration deleted");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test failed migration evidence and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_failed_migration_evidence(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutMigrationStoreRepository repository;
    UmiWorkbenchLayoutMigrationRecord record = make_migration_record(
        "migration-failed", 2U, 3U,
        UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_FAILED);
    UmiWorkbenchLayoutMigrationRecord loaded;
    TEST_REQUIRE(server != NULL, "create server");
    test_copy_text(record.error, sizeof(record.error),
                   "migration validation failed");
    TEST_STATUS_OK(umi_workbench_layout_migration_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_migration_store_save(
        &repository, &record));
    TEST_STATUS_OK(umi_workbench_layout_migration_store_load(
        &repository, NULL, "migration-failed", 0U, &loaded));
    TEST_REQUIRE(strcmp(loaded.error,
        "migration validation failed") == 0,
        "failure evidence retained");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_migration_record_codec() == 0,
                 "migration codec");
    TEST_REQUIRE(test_migration_repository() == 0,
                 "migration repository");
    TEST_REQUIRE(test_failed_migration_evidence() == 0,
                 "migration failure evidence");
    return 0;
}
