/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_reconciliation.c
 *
 * PURPOSE:
 *   Verify Data Server manifest/chunk reconciliation, issue capacity, orphan detection and safe repair evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_issue_builder(void)
{
    UmiWorkbenchLayoutReconciliationReport report;
    (void)memset(&report, 0, sizeof(report));
    report.structure_size = sizeof(report);
    TEST_STATUS_OK(umi_workbench_layout_reconciliation_add_issue(
        &report,
        "workbench-layout/layout-chunk/layout.a/9",
        UMI_STATUS_NOT_FOUND,
        true,
        "orphan chunk",
        1000U));
    TEST_REQUIRE(report.issue_count == 1U, "issue added");
    TEST_REQUIRE(report.issues[0].repairable,
                 "repairability retained");
    TEST_REQUIRE(strcmp(report.issues[0].message, "orphan chunk") == 0,
                 "issue message retained");
    return 0;
}

static int test_consistent_store(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChunkStore store;
    UmiWorkbenchLayoutReconciliationPolicy policy =
        umi_workbench_layout_reconciliation_policy_default();
    UmiWorkbenchLayoutReconciliationReport report;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_init(
        &store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK));
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_save(
        &store, "layout.consistent", "payload", 1U, 1000U, NULL));
    TEST_STATUS_OK(umi_workbench_layout_reconcile(
        server, &policy, 2000U, &report));
    TEST_REQUIRE(report.consistent, "store consistent");
    TEST_REQUIRE(report.manifest_count == 1U,
                 "manifest counted");
    TEST_REQUIRE(report.chunk_count == 1U, "chunk counted");
    TEST_REQUIRE(report.issue_count == 0U, "no issues");
    umi_data_server_destroy(server);
    return 0;
}

static int test_orphan_detection(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutReconciliationPolicy policy =
        umi_workbench_layout_reconciliation_policy_default();
    UmiWorkbenchLayoutReconciliationReport report;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/layout-chunk/layout.orphan/0",
        "orphan"));
    policy.repair_orphan_chunks = false;
    TEST_STATUS_OK(umi_workbench_layout_reconcile(
        server, &policy, 2000U, &report));
    TEST_REQUIRE(!report.consistent, "orphan makes store inconsistent");
    TEST_REQUIRE(report.orphan_count == 1U, "orphan counted");
    TEST_REQUIRE(report.issue_count >= 1U, "orphan issue emitted");
    umi_data_server_destroy(server);
    return 0;
}

static int test_orphan_repair(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutReconciliationPolicy policy =
        umi_workbench_layout_reconciliation_policy_default();
    UmiWorkbenchLayoutReconciliationReport report;
    char value[32];
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/layout-chunk/layout.orphan/0",
        "orphan"));
    policy.repair_orphan_chunks = true;
    TEST_STATUS_OK(umi_workbench_layout_reconcile(
        server, &policy, 2000U, &report));
    TEST_REQUIRE(report.repaired_count == 1U,
                 "orphan repaired");
    TEST_REQUIRE(umi_data_server_get(
        server, "workbench-layout/layout-chunk/layout.orphan/0",
        value, sizeof(value)) == UMI_STATUS_NOT_FOUND,
        "orphan removed");
    umi_data_server_destroy(server);
    return 0;
}

static int test_unknown_key_detection(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutReconciliationPolicy policy =
        umi_workbench_layout_reconciliation_policy_default();
    UmiWorkbenchLayoutReconciliationReport report;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/unknown/value", "unknown"));
    policy.detect_unknown_keys = true;
    TEST_STATUS_OK(umi_workbench_layout_reconcile(
        server, &policy, 2000U, &report));
    TEST_REQUIRE(report.issue_count >= 1U,
                 "unknown key reported");
    TEST_REQUIRE(!report.consistent,
                 "unknown owned key degrades consistency");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_issue_builder() == 0, "issue builder");
    TEST_REQUIRE(test_consistent_store() == 0, "consistent store");
    TEST_REQUIRE(test_orphan_detection() == 0,
                 "orphan detection");
    TEST_REQUIRE(test_orphan_repair() == 0, "orphan repair");
    TEST_REQUIRE(test_unknown_key_detection() == 0,
                 "unknown key");
    return 0;
}
