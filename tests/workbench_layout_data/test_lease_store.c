/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_lease_store.c
 *
 * PURPOSE:
 *   Verify exclusive edit leases, monotonic fencing tokens, renewal, release, expiry and write-authority checks.
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
 * Exercise test lease codec and return a clear result when the behaviour no longer matches
 * its contract.
 */
static int test_lease_codec(void)
{
    UmiWorkbenchLayoutLease source;
    UmiWorkbenchLayoutLease decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    test_copy_text(source.lease_id, sizeof(source.lease_id), "lease-codec");
    test_copy_text(source.layout_id, sizeof(source.layout_id), "layout.edit");
    test_copy_text(source.holder_id, sizeof(source.holder_id), "developer");
    test_copy_text(source.client_id, sizeof(source.client_id), "client-a");
    source.state = UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD;
    source.fencing_token = 7U;
    source.acquired_at_ms = 1000U;
    source.renewed_at_ms = 1100U;
    source.expires_at_ms = 5000U;
    source.revision = 2U;
    TEST_STATUS_OK(umi_workbench_layout_lease_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_lease_store_decode(value, &decoded));
    TEST_REQUIRE(decoded.state == UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD,
                 "lease state round trip");
    TEST_REQUIRE(decoded.fencing_token == 7U,
                 "fencing token round trip");
    TEST_REQUIRE(strcmp(decoded.client_id, "client-a") == 0,
                 "client identity round trip");
    return 0;
}

/*
 * Exercise test acquire and write authority and return a clear result when the behaviour
 * no longer matches its contract.
 */
static int test_acquire_and_write_authority(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutLeaseStoreRepository repository;
    UmiWorkbenchLayoutLease lease;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_lease_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_lease_store_acquire(
        &repository, "layout.edit", "developer", "client-a",
        1000U, 5000U, &lease));
    TEST_REQUIRE(lease.state == UMI_WORKBENCH_LAYOUT_DATA_LEASE_HELD,
                 "lease held");
    TEST_REQUIRE(lease.fencing_token > 0U,
                 "fencing token assigned");
    TEST_REQUIRE(umi_workbench_layout_lease_store_allows_write(
        &lease, "developer", "client-a", lease.fencing_token, 2000U),
        "holder may write");
    TEST_REQUIRE(!umi_workbench_layout_lease_store_allows_write(
        &lease, "other", "client-a", lease.fencing_token, 2000U),
        "other holder denied");
    TEST_REQUIRE(!umi_workbench_layout_lease_store_allows_write(
        &lease, "developer", "client-a", lease.fencing_token - 1U, 2000U),
        "stale fencing token denied");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test competing acquisition and expiry and return a clear result when the
 * behaviour no longer matches its contract.
 */
static int test_competing_acquisition_and_expiry(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutLeaseStoreRepository repository;
    UmiWorkbenchLayoutLease first;
    UmiWorkbenchLayoutLease second;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_lease_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_lease_store_acquire(
        &repository, "layout.edit", "developer", "client-a",
        1000U, 1000U, &first));
    TEST_REQUIRE(umi_workbench_layout_lease_store_acquire(
        &repository, "layout.edit", "other", "client-b",
        1500U, 1000U, &second) == UMI_STATUS_BUSY,
        "active competing lease rejected");
    TEST_STATUS_OK(umi_workbench_layout_lease_store_acquire(
        &repository, "layout.edit", "other", "client-b",
        2000U, 1000U, &second));
    TEST_REQUIRE(second.fencing_token > first.fencing_token,
                 "new lease advances fencing token");
    TEST_REQUIRE(!umi_workbench_layout_lease_store_allows_write(
        &first, "developer", "client-a", first.fencing_token, 2000U),
        "expired old lease denied");
    umi_data_server_destroy(server);
    return 0;
}

/* Release or reset state held by test renew and so the same storage can be reused safely. */
static int test_renew_and_release(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutLeaseStoreRepository repository;
    UmiWorkbenchLayoutLease lease;
    uint64_t original_expiry;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_lease_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_lease_store_acquire(
        &repository, "layout.edit", "developer", "client-a",
        1000U, 1000U, &lease));
    original_expiry = lease.expires_at_ms;
    TEST_STATUS_OK(umi_workbench_layout_lease_store_renew(
        &repository, &lease, 1500U, 5000U));
    TEST_REQUIRE(lease.expires_at_ms > original_expiry,
                 "renewal extends expiry");
    TEST_REQUIRE(lease.revision > 1U, "renewal advances revision");
    TEST_STATUS_OK(umi_workbench_layout_lease_store_release(
        &repository, &lease, 1600U));
    TEST_REQUIRE(lease.state == UMI_WORKBENCH_LAYOUT_DATA_LEASE_RELEASED,
                 "release state persisted");
    TEST_REQUIRE(!umi_workbench_layout_lease_store_allows_write(
        &lease, "developer", "client-a", lease.fencing_token, 1700U),
        "released lease denied");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_lease_codec() == 0, "lease codec");
    TEST_REQUIRE(test_acquire_and_write_authority() == 0,
                 "acquire authority");
    TEST_REQUIRE(test_competing_acquisition_and_expiry() == 0,
                 "competing expiry");
    TEST_REQUIRE(test_renew_and_release() == 0, "renew release");
    return 0;
}
