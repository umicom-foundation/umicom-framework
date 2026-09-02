/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_capacity_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded key, value, chunk, field, page, issue and sync-plan capacities without truncation or silent overflow.
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
 * Exercise test key capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_key_capacity(void)
{
    char key[16];
    TEST_REQUIRE(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        "layout.capacity", NULL, 0U, 0U,
        key, sizeof(key)) == UMI_STATUS_CAPACITY_EXCEEDED,
        "small key buffer rejected");
    TEST_REQUIRE(key[0] == '\0', "failed key buffer cleared");
    return 0;
}

/*
 * Exercise test value capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_value_capacity(void)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    char buffer[8];
    size_t required = 0U;
    umi_workbench_layout_data_field_set_init(&fields);
    TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
        &fields, "field", "substantial-value"));
    TEST_REQUIRE(umi_workbench_layout_data_value_encode(
        &fields, buffer, sizeof(buffer), &required) ==
        UMI_STATUS_CAPACITY_EXCEEDED,
        "small value buffer rejected");
    TEST_REQUIRE(required > sizeof(buffer),
                 "required value capacity reported");
    return 0;
}

/*
 * Exercise test field capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_field_capacity(void)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    size_t index;
    char key[32];
    umi_workbench_layout_data_field_set_init(&fields);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < sizeof(fields.fields) / sizeof(fields.fields[0]);
         ++index) {
        (void)snprintf(key, sizeof(key), "field-%zu", index);
        TEST_STATUS_OK(umi_workbench_layout_data_field_set_put(
            &fields, key, "value"));
    }
    TEST_REQUIRE(umi_workbench_layout_data_field_set_put(
        &fields, "overflow", "value") ==
        UMI_STATUS_CAPACITY_EXCEEDED, "field capacity enforced");
    return 0;
}

/*
 * Exercise test chunk capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_chunk_capacity(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChunkStore store;
    const size_t size =
        (UMI_WORKBENCH_LAYOUT_DATA_MAX_CHUNKS + 1U) * 3600U;
    char *payload = (char *)malloc(size + 1U);
    TEST_REQUIRE(server != NULL && payload != NULL,
                 "allocate chunk capacity fixture");
    (void)memset(payload, 'x', size);
    payload[size] = '\0';
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_init(
        &store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK));
    TEST_REQUIRE(umi_workbench_layout_chunk_store_save(
        &store, "layout.too-large", payload, 1U, 1000U, NULL) ==
        UMI_STATUS_CAPACITY_EXCEEDED,
        "chunk count capacity enforced");
    free(payload);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test issue capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_issue_capacity(void)
{
    UmiWorkbenchLayoutReconciliationReport report;
    size_t index;
    char key[64];
    (void)memset(&report, 0, sizeof(report));
    report.structure_size = sizeof(report);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_DATA_MAX_RECONCILIATION_ISSUES;
         ++index) {
        (void)snprintf(key, sizeof(key), "key-%zu", index);
        TEST_STATUS_OK(umi_workbench_layout_reconciliation_add_issue(
            &report, key, UMI_STATUS_PARSE_ERROR,
            false, "issue", 1000U));
    }
    TEST_REQUIRE(umi_workbench_layout_reconciliation_add_issue(
        &report, "overflow", UMI_STATUS_PARSE_ERROR,
        false, "issue", 1000U) ==
        UMI_STATUS_CAPACITY_EXCEEDED, "issue capacity enforced");
    return 0;
}

/*
 * Exercise test sync plan capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_sync_plan_capacity(void)
{
    UmiWorkbenchLayoutSyncPlan plan;
    UmiWorkbenchLayoutSyncPlanItem item;
    size_t index;
    (void)memset(&plan, 0, sizeof(plan));
    plan.structure_size = sizeof(plan);
    test_copy_text(plan.plan_id, sizeof(plan.plan_id), "capacity-plan");
    test_copy_text(plan.local_replica_id,
                   sizeof(plan.local_replica_id), "local");
    test_copy_text(plan.remote_replica_id,
                   sizeof(plan.remote_replica_id), "remote");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_LAYOUT_DATA_MAX_SYNC_ITEMS;
         ++index) {
        (void)memset(&item, 0, sizeof(item));
        item.structure_size = sizeof(item);
        (void)snprintf(item.layout_id,
                       sizeof(item.layout_id), "layout-%zu", index);
        item.action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE;
        TEST_STATUS_OK(umi_workbench_layout_sync_plan_add_item(
            &plan, &item));
    }
    (void)memset(&item, 0, sizeof(item));
    item.structure_size = sizeof(item);
    test_copy_text(item.layout_id, sizeof(item.layout_id), "overflow");
    item.action = UMI_WORKBENCH_LAYOUT_DATA_SYNC_CREATE_REMOTE;
    TEST_REQUIRE(umi_workbench_layout_sync_plan_add_item(
        &plan, &item) == UMI_STATUS_CAPACITY_EXCEEDED,
        "sync plan capacity enforced");
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_key_capacity() == 0, "key capacity");
    TEST_REQUIRE(test_value_capacity() == 0, "value capacity");
    TEST_REQUIRE(test_field_capacity() == 0, "field capacity");
    TEST_REQUIRE(test_chunk_capacity() == 0, "chunk capacity");
    TEST_REQUIRE(test_issue_capacity() == 0, "issue capacity");
    TEST_REQUIRE(test_sync_plan_capacity() == 0,
                 "sync plan capacity");
    return 0;
}
