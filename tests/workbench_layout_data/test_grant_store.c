/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_grant_store.c
 *
 * PURPOSE:
 *   Verify persistent time-bounded sharing grants, permission evaluation, expiry and revocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_grant_codec(void)
{
    UmiWorkbenchLayoutGrant source = test_create_grant(
        "grant-codec", "layout.shared", "analyst",
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW) |
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT));
    UmiWorkbenchLayoutGrant decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_grant_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_decode(
        value, &decoded));
    TEST_REQUIRE(strcmp(decoded.grant_id, "grant-codec") == 0,
                 "grant id round trip");
    TEST_REQUIRE(strcmp(decoded.subject_id, "analyst") == 0,
                 "subject round trip");
    TEST_REQUIRE(decoded.permissions == source.permissions,
                 "permissions round trip");
    TEST_REQUIRE(!decoded.revoked, "revocation state round trip");
    return 0;
}

static int test_permission_evaluation(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutGrantStoreRepository repository;
    UmiWorkbenchLayoutGrant grant = test_create_grant(
        "grant-view-edit", "layout.shared", "analyst",
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW) |
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT));
    bool allowed = false;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_save(
        &repository, &grant));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        &repository, "layout.shared", "analyst",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW, 5000U, &allowed));
    TEST_REQUIRE(allowed, "view permission allowed");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        &repository, "layout.shared", "analyst",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT, 5000U, &allowed));
    TEST_REQUIRE(allowed, "edit permission allowed");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        &repository, "layout.shared", "analyst",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_DELETE, 5000U, &allowed));
    TEST_REQUIRE(!allowed, "ungranted permission denied");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        &repository, "layout.shared", "other",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW, 5000U, &allowed));
    TEST_REQUIRE(!allowed, "other subject denied");
    umi_data_server_destroy(server);
    return 0;
}

static int test_expiry_and_revocation(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutGrantStoreRepository repository;
    UmiWorkbenchLayoutGrant grant = test_create_grant(
        "grant-expiring", "layout.shared", "analyst",
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW));
    UmiWorkbenchLayoutGrant loaded;
    bool allowed = true;
    TEST_REQUIRE(server != NULL, "create server");
    grant.expires_at_ms = 2000U;
    TEST_STATUS_OK(umi_workbench_layout_grant_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_save(
        &repository, &grant));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        &repository, "layout.shared", "analyst",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW, 2000U, &allowed));
    TEST_REQUIRE(!allowed, "expired grant denied");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_revoke(
        &repository, "layout.shared", "grant-expiring"));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_load(
        &repository, "layout.shared", "grant-expiring", 0U, &loaded));
    TEST_REQUIRE(loaded.revoked, "revocation persisted");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        &repository, "layout.shared", "analyst",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW, 1500U, &allowed));
    TEST_REQUIRE(!allowed, "revoked grant denied before expiry");
    umi_data_server_destroy(server);
    return 0;
}

static int test_grant_listing(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutGrantStoreRepository repository;
    UmiWorkbenchLayoutGrant first = test_create_grant(
        "grant-one", "layout.shared", "analyst", 2U);
    UmiWorkbenchLayoutGrant second = test_create_grant(
        "grant-two", "layout.shared", "reviewer", 2U);
    UmiWorkbenchLayoutGrant results[4];
    size_t count = 0U;
    size_t total = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_save(
        &repository, &second));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_list(
        &repository, "layout.shared", NULL, NULL,
        results, 4U, &count, &total));
    TEST_REQUIRE(count == 2U && total == 2U,
                 "all grants listed");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_grant_codec() == 0, "grant codec");
    TEST_REQUIRE(test_permission_evaluation() == 0,
                 "permission evaluation");
    TEST_REQUIRE(test_expiry_and_revocation() == 0,
                 "expiry revocation");
    TEST_REQUIRE(test_grant_listing() == 0, "grant listing");
    return 0;
}
