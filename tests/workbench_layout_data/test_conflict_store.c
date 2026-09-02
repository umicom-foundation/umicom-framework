/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_conflict_store.c
 *
 * PURPOSE:
 *   Verify persistent synchronisation conflicts, open-conflict counts and explicit resolution evidence.
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
 * Exercise make conflict and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiWorkbenchLayoutConflict make_conflict(
    const char *conflict_id,
    const char *layout_id,
    UmiWorkbenchLayoutDataConflictKind kind)
{
    UmiWorkbenchLayoutConflict conflict;
    (void)memset(&conflict, 0, sizeof(conflict));
    conflict.structure_size = sizeof(conflict);
    test_copy_text(conflict.conflict_id,
                   sizeof(conflict.conflict_id), conflict_id);
    test_copy_text(conflict.layout_id,
                   sizeof(conflict.layout_id), layout_id);
    test_copy_text(conflict.local_replica_id,
                   sizeof(conflict.local_replica_id), "local");
    test_copy_text(conflict.remote_replica_id,
                   sizeof(conflict.remote_replica_id), "remote");
    conflict.kind = kind;
    conflict.state = UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN;
    conflict.base_revision = 1U;
    conflict.local_revision = 2U;
    conflict.remote_revision = 3U;
    conflict.created_at_ms = 1000U;
    test_copy_text(conflict.message, sizeof(conflict.message),
                   "concurrent layout update");
    return conflict;
}

/*
 * Exercise test conflict codec and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_conflict_codec(void)
{
    UmiWorkbenchLayoutConflict source = make_conflict(
        "conflict-codec", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE);
    UmiWorkbenchLayoutConflict decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_decode(
        value, &decoded));
    TEST_REQUIRE(decoded.kind ==
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE,
        "conflict kind round trip");
    TEST_REQUIRE(decoded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN,
        "conflict state round trip");
    TEST_REQUIRE(decoded.remote_revision == 3U,
        "remote revision round trip");
    return 0;
}

/* Return the number of records represented by test open without changing their state. */
static int test_open_count(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository repository;
    UmiWorkbenchLayoutConflict first = make_conflict(
        "conflict-one", "layout.a",
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE);
    UmiWorkbenchLayoutConflict second = make_conflict(
        "conflict-two", "layout.b",
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_INTEGRITY);
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_save(
        &repository, &second));
    TEST_REQUIRE(umi_workbench_layout_conflict_store_open_count(
        &repository) == 2U, "open conflicts counted");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_resolve(
        &repository, &first,
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED,
        "developer", 2000U));
    TEST_REQUIRE(umi_workbench_layout_conflict_store_open_count(
        &repository) == 1U, "resolved conflict excluded");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test resolution evidence and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_resolution_evidence(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository repository;
    UmiWorkbenchLayoutConflict conflict = make_conflict(
        "conflict-resolve", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_DELETE_VERSUS_UPDATE);
    UmiWorkbenchLayoutConflict loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_save(
        &repository, &conflict));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_resolve(
        &repository, &conflict,
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_REMOTE,
        "reviewer", 2500U));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_load(
        &repository, "layout.shared", "conflict-resolve", 0U, &loaded));
    TEST_REQUIRE(loaded.state ==
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_REMOTE,
        "resolution persisted");
    TEST_REQUIRE(strcmp(loaded.resolved_by, "reviewer") == 0,
        "resolver retained");
    TEST_REQUIRE(loaded.resolved_at_ms == 2500U,
        "resolution time retained");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test invalid resolution and return a clear result when the behaviour no longer
 * matches its contract.
 */
static int test_invalid_resolution(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutConflictStoreRepository repository;
    UmiWorkbenchLayoutConflict conflict = make_conflict(
        "conflict-invalid", "layout.shared",
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_INTEGRITY);
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_save(
        &repository, &conflict));
    TEST_REQUIRE(umi_workbench_layout_conflict_store_resolve(
        &repository, &conflict,
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN,
        "developer", 2000U) == UMI_STATUS_INVALID_ARGUMENT,
        "open state cannot resolve conflict");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_conflict_codec() == 0, "conflict codec");
    TEST_REQUIRE(test_open_count() == 0, "open count");
    TEST_REQUIRE(test_resolution_evidence() == 0,
                 "resolution evidence");
    TEST_REQUIRE(test_invalid_resolution() == 0,
                 "invalid resolution");
    return 0;
}
