/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_collaboration_access_lifecycle.c
 *
 * PURPOSE:
 *   Verify that sharing grants, edit leases, collaboration presence and
 *   conflict resolution cooperate through one authoritative Data Server.
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
 * Exercise make presence and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiWorkbenchLayoutPresence make_presence(
    const char *presence_id,
    const char *user_id,
    const char *client_id,
    uint64_t now_ms,
    uint64_t ttl_ms)
{
    UmiWorkbenchLayoutPresence presence;
    (void)memset(&presence, 0, sizeof(presence));
    presence.structure_size = sizeof(presence);
    test_copy_text(
        presence.presence_id, sizeof(presence.presence_id), presence_id);
    test_copy_text(
        presence.layout_id, sizeof(presence.layout_id),
        "layout.collaboration");
    test_copy_text(presence.user_id, sizeof(presence.user_id), user_id);
    test_copy_text(presence.client_id, sizeof(presence.client_id), client_id);
    test_copy_text(
        presence.active_node_id, sizeof(presence.active_node_id), "editor");
    presence.connected_at_ms = now_ms;
    presence.last_seen_at_ms = now_ms;
    presence.expires_at_ms = now_ms + ttl_ms;
    presence.editing = true;
    presence.synchronising = false;
    return presence;
}

/*
 * Exercise make conflict and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiWorkbenchLayoutConflict make_conflict(void)
{
    UmiWorkbenchLayoutConflict conflict;
    (void)memset(&conflict, 0, sizeof(conflict));
    conflict.structure_size = sizeof(conflict);
    test_copy_text(
        conflict.conflict_id, sizeof(conflict.conflict_id),
        "conflict-collaboration");
    test_copy_text(
        conflict.layout_id, sizeof(conflict.layout_id),
        "layout.collaboration");
    test_copy_text(
        conflict.local_replica_id, sizeof(conflict.local_replica_id),
        "replica-local");
    test_copy_text(
        conflict.remote_replica_id, sizeof(conflict.remote_replica_id),
        "replica-remote");
    conflict.kind = UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_CONCURRENT_UPDATE;
    conflict.state = UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_OPEN;
    conflict.base_revision = 3U;
    conflict.local_revision = 4U;
    conflict.remote_revision = 5U;
    conflict.created_at_ms = 4000U;
    test_copy_text(
        conflict.message, sizeof(conflict.message),
        "Concurrent layout edits require an explicit merge decision.");
    return conflict;
}

/*
 * Exercise test grant and lease authority and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_grant_and_lease_authority(
    UmiDataServer *server,
    UmiWorkbenchLayoutGrantStoreRepository *grants,
    UmiWorkbenchLayoutLeaseStoreRepository *leases)
{
    const uint32_t permissions =
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW) |
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT) |
        (1U << UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_SHARE);
    UmiWorkbenchLayoutGrant grant = test_create_grant(
        "grant-collaboration", "layout.collaboration", "reviewer",
        permissions);
    UmiWorkbenchLayoutLease first;
    UmiWorkbenchLayoutLease competing;
    bool allowed = false;
    (void)server;

    TEST_STATUS_OK(umi_workbench_layout_grant_store_save(grants, &grant));
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        grants, "layout.collaboration", "reviewer",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_VIEW, 2000U, &allowed));
    TEST_REQUIRE(allowed, "reviewer can view shared layout");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        grants, "layout.collaboration", "reviewer",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_EDIT, 2000U, &allowed));
    TEST_REQUIRE(allowed, "reviewer can edit shared layout");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_evaluate(
        grants, "layout.collaboration", "reviewer",
        UMI_WORKBENCH_LAYOUT_DATA_PERMISSION_DELETE, 2000U, &allowed));
    TEST_REQUIRE(!allowed, "delete remains outside the grant");

    TEST_STATUS_OK(umi_workbench_layout_lease_store_acquire(
        leases, "layout.collaboration", "reviewer", "review-client",
        2000U, 2000U, &first));
    TEST_REQUIRE(first.fencing_token > 0U, "fencing token assigned");
    TEST_REQUIRE(umi_workbench_layout_lease_store_allows_write(
        &first, "reviewer", "review-client", first.fencing_token, 2500U),
        "current lease holder can write");
    TEST_REQUIRE(umi_workbench_layout_lease_store_acquire(
        leases, "layout.collaboration", "developer", "author-client",
        2500U, 2000U, &competing) == UMI_STATUS_BUSY,
        "competing writer is rejected while lease is active");

    TEST_STATUS_OK(umi_workbench_layout_lease_store_renew(
        leases, &first, 3000U, 3000U));
    TEST_REQUIRE(first.expires_at_ms == 6000U, "lease renewal persisted");
    TEST_STATUS_OK(umi_workbench_layout_lease_store_release(
        leases, &first, 3500U));
    TEST_REQUIRE(!umi_workbench_layout_lease_store_allows_write(
        &first, "reviewer", "review-client", first.fencing_token, 3600U),
        "released lease cannot authorise writes");
    return 0;
}

/*
 * Exercise test presence and conflict lifecycle and return a clear result when the
 * behaviour no longer matches its contract.
 */
static int test_presence_and_conflict_lifecycle(
    UmiWorkbenchLayoutPresenceStoreRepository *presence,
    UmiWorkbenchLayoutConflictStoreRepository *conflicts)
{
    UmiWorkbenchLayoutPresence author = make_presence(
        "presence-author", "developer", "author-client", 1000U, 5000U);
    UmiWorkbenchLayoutPresence reviewer = make_presence(
        "presence-reviewer", "reviewer", "review-client", 1000U, 1500U);
    UmiWorkbenchLayoutConflict conflict = make_conflict();
    UmiWorkbenchLayoutConflict loaded;
    size_t expired = 0U;

    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        presence, &author));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        presence, &reviewer));
    TEST_REQUIRE(umi_workbench_layout_presence_store_active_count(
        presence, 2000U) == 2U, "both collaborators are active");

    TEST_STATUS_OK(umi_workbench_layout_presence_store_heartbeat(
        presence, &author, "output", false, true, 2200U, 5000U));
    TEST_REQUIRE(strcmp(author.active_node_id, "output") == 0,
                 "heartbeat updates active node");
    TEST_REQUIRE(!author.editing && author.synchronising,
                 "heartbeat updates collaboration activity");

    TEST_STATUS_OK(umi_workbench_layout_presence_store_expire(
        presence, 3000U, &expired));
    TEST_REQUIRE(expired == 1U, "expired collaborator removed");
    TEST_REQUIRE(umi_workbench_layout_presence_store_active_count(
        presence, 3000U) == 1U, "active collaborator retained");

    TEST_STATUS_OK(umi_workbench_layout_conflict_store_save(
        conflicts, &conflict));
    TEST_REQUIRE(umi_workbench_layout_conflict_store_open_count(
        conflicts) == 1U, "open conflict recorded");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_resolve(
        conflicts, &conflict,
        UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED,
        "developer", 5000U));
    TEST_REQUIRE(umi_workbench_layout_conflict_store_open_count(
        conflicts) == 0U, "resolved conflict leaves open set");
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_load(
        conflicts, "layout.collaboration", "conflict-collaboration",
        0U, &loaded));
    TEST_REQUIRE(loaded.state ==
                     UMI_WORKBENCH_LAYOUT_DATA_CONFLICT_RESOLVED_MERGED,
                 "merged resolution persisted");
    TEST_REQUIRE(strcmp(loaded.resolved_by, "developer") == 0,
                 "resolver identity retained");
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutGrantStoreRepository grants;
    UmiWorkbenchLayoutLeaseStoreRepository leases;
    UmiWorkbenchLayoutPresenceStoreRepository presence;
    UmiWorkbenchLayoutConflictStoreRepository conflicts;

    TEST_REQUIRE(server != NULL, "create authoritative Data Server");
    TEST_STATUS_OK(umi_workbench_layout_grant_store_repository_init(
        &grants, server));
    TEST_STATUS_OK(umi_workbench_layout_lease_store_repository_init(
        &leases, server));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_repository_init(
        &presence, server));
    TEST_STATUS_OK(umi_workbench_layout_conflict_store_repository_init(
        &conflicts, server));

    TEST_REQUIRE(test_grant_and_lease_authority(
                     server, &grants, &leases) == 0,
                 "grant and lease authority");
    TEST_REQUIRE(test_presence_and_conflict_lifecycle(
                     &presence, &conflicts) == 0,
                 "presence and conflict lifecycle");
    TEST_REQUIRE(umi_data_server_count(server) >= 4U,
                 "collaboration evidence stored by Data Server");

    umi_data_server_destroy(server);
    return 0;
}
