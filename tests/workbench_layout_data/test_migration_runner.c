/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_migration_runner.c
 *
 * PURPOSE:
 *   Verify ordered, restartable and transactional schema migrations with version evidence and rollback after failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

typedef struct MigrationContext {
    const char *key;
    const char *value;
    bool fail;
    size_t apply_count;
    size_t rollback_count;
} MigrationContext;

static UmiStatus apply_migration(UmiDataServer *server, void *context_value)
{
    MigrationContext *context = (MigrationContext *)context_value;
    if (server == NULL || context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->apply_count += 1U;
    if (context->fail) return UMI_STATUS_INTERNAL_ERROR;
    return umi_data_server_set(server, context->key, context->value);
}

static UmiStatus rollback_migration(
    UmiDataServer *server,
    void *context_value)
{
    MigrationContext *context = (MigrationContext *)context_value;
    UmiStatus status;
    if (server == NULL || context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context->rollback_count += 1U;
    status = umi_data_server_delete(server, context->key);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}

static UmiWorkbenchLayoutDataMigrationStep migration_step(
    const char *migration_id,
    uint32_t from_version,
    uint32_t to_version,
    MigrationContext *context)
{
    UmiWorkbenchLayoutDataMigrationStep step;
    (void)memset(&step, 0, sizeof(step));
    step.structure_size = sizeof(step);
    step.descriptor.structure_size = sizeof(step.descriptor);
    test_copy_text(step.descriptor.migration_id,
                   sizeof(step.descriptor.migration_id), migration_id);
    test_copy_text(step.descriptor.name,
                   sizeof(step.descriptor.name), migration_id);
    step.descriptor.from_version = from_version;
    step.descriptor.to_version = to_version;
    step.descriptor.checksum =
        umi_workbench_layout_data_hash_text(migration_id);
    step.descriptor.reversible = true;
    step.apply = apply_migration;
    step.rollback = rollback_migration;
    step.context = context;
    return step;
}

static int test_version_read_write(void)
{
    UmiDataServer *server = test_create_data_server();
    uint32_t version = 99U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_migration_read_version(
        server, &version));
    TEST_REQUIRE(version == 0U, "missing version starts at zero");
    TEST_STATUS_OK(umi_workbench_layout_migration_write_version(
        server, 3U));
    TEST_STATUS_OK(umi_workbench_layout_migration_read_version(
        server, &version));
    TEST_REQUIRE(version == 3U, "version round trip");
    umi_data_server_destroy(server);
    return 0;
}

static int test_successful_plan(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutMigrationStoreRepository records;
    UmiWorkbenchLayoutDataMigrationPlan plan;
    UmiWorkbenchLayoutMigrationResult result;
    MigrationContext first = {"schema/one", "ready", false, 0U, 0U};
    MigrationContext second = {"schema/two", "ready", false, 0U, 0U};
    UmiWorkbenchLayoutDataMigrationStep first_step = migration_step(
        "migration-one", 0U, 1U, &first);
    UmiWorkbenchLayoutDataMigrationStep second_step = migration_step(
        "migration-two", 1U, 2U, &second);
    uint32_t version = 0U;
    char value[32];
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_migration_store_repository_init(
        &records, server));
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_init(
        &plan, 0U, 2U));
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_add(
        &plan, &first_step));
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_add(
        &plan, &second_step));
    TEST_STATUS_OK(umi_workbench_layout_migration_execute(
        server, &records, &plan, "developer", 1000U, &result));
    TEST_REQUIRE(result.applied_count == 2U,
                 "both migrations applied");
    TEST_REQUIRE(result.resulting_version == 2U,
                 "target version reached");
    TEST_REQUIRE(first.apply_count == 1U && second.apply_count == 1U,
                 "apply callbacks invoked once");
    TEST_STATUS_OK(umi_workbench_layout_migration_read_version(
        server, &version));
    TEST_REQUIRE(version == 2U, "version persisted");
    TEST_STATUS_OK(umi_data_server_get(
        server, "schema/two", value, sizeof(value)));
    TEST_REQUIRE(strcmp(value, "ready") == 0,
                 "migration data committed");
    umi_data_server_destroy(server);
    return 0;
}

static int test_failure_rolls_back(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutMigrationStoreRepository records;
    UmiWorkbenchLayoutDataMigrationPlan plan;
    UmiWorkbenchLayoutMigrationResult result;
    MigrationContext first = {"schema/one", "ready", false, 0U, 0U};
    MigrationContext second = {"schema/two", "ready", true, 0U, 0U};
    UmiWorkbenchLayoutDataMigrationStep first_step = migration_step(
        "migration-one", 0U, 1U, &first);
    UmiWorkbenchLayoutDataMigrationStep second_step = migration_step(
        "migration-two", 1U, 2U, &second);
    char value[32];
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_migration_store_repository_init(
        &records, server));
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_init(
        &plan, 0U, 2U));
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_add(
        &plan, &first_step));
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_add(
        &plan, &second_step));
    TEST_REQUIRE(umi_workbench_layout_migration_execute(
        server, &records, &plan, "developer", 1000U, &result) ==
        UMI_STATUS_INTERNAL_ERROR, "failed migration propagated");
    TEST_REQUIRE(result.applied_count == 1U,
                 "first migration reached apply stage");
    TEST_REQUIRE(result.rollback_count >= 1U,
                 "rollback evidence recorded");
    TEST_REQUIRE(umi_data_server_get(
        server, "schema/one", value, sizeof(value)) ==
        UMI_STATUS_NOT_FOUND, "transaction rollback removed changes");
    TEST_REQUIRE(strcmp(result.failed_migration_id,
        "migration-two") == 0, "failed migration identified");
    umi_data_server_destroy(server);
    return 0;
}

static int test_plan_order_validation(void)
{
    UmiWorkbenchLayoutDataMigrationPlan plan;
    MigrationContext context = {"key", "value", false, 0U, 0U};
    UmiWorkbenchLayoutDataMigrationStep step = migration_step(
        "bad-order", 2U, 3U, &context);
    TEST_STATUS_OK(umi_workbench_layout_migration_plan_init(
        &plan, 0U, 3U));
    TEST_REQUIRE(umi_workbench_layout_migration_plan_add(
        &plan, &step) == UMI_STATUS_INVALID_STATE,
        "non-contiguous migration rejected");
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_version_read_write() == 0,
                 "version read write");
    TEST_REQUIRE(test_successful_plan() == 0,
                 "successful migration");
    TEST_REQUIRE(test_failure_rolls_back() == 0,
                 "migration rollback");
    TEST_REQUIRE(test_plan_order_validation() == 0,
                 "plan order");
    return 0;
}
