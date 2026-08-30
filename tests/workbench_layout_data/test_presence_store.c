/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_presence_store.c
 *
 * PURPOSE:
 *   Verify collaboration heartbeats, active editor presence, expiry cleanup and active-user counting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiWorkbenchLayoutPresence make_presence(
    const char *presence_id,
    const char *layout_id,
    const char *user_id,
    const char *client_id,
    uint64_t now_ms,
    uint64_t ttl_ms)
{
    UmiWorkbenchLayoutPresence presence;
    (void)memset(&presence, 0, sizeof(presence));
    presence.structure_size = sizeof(presence);
    test_copy_text(presence.presence_id,
                   sizeof(presence.presence_id), presence_id);
    test_copy_text(presence.layout_id,
                   sizeof(presence.layout_id), layout_id);
    test_copy_text(presence.user_id,
                   sizeof(presence.user_id), user_id);
    test_copy_text(presence.client_id,
                   sizeof(presence.client_id), client_id);
    test_copy_text(presence.active_node_id,
                   sizeof(presence.active_node_id), "editor");
    presence.connected_at_ms = now_ms;
    presence.last_seen_at_ms = now_ms;
    presence.expires_at_ms = now_ms + ttl_ms;
    presence.editing = true;
    return presence;
}

static int test_presence_codec(void)
{
    UmiWorkbenchLayoutPresence source = make_presence(
        "presence-codec", "layout.shared", "developer", "client-a",
        1000U, 5000U);
    UmiWorkbenchLayoutPresence decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_presence_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_decode(
        value, &decoded));
    TEST_REQUIRE(strcmp(decoded.presence_id, "presence-codec") == 0,
                 "presence id round trip");
    TEST_REQUIRE(strcmp(decoded.active_node_id, "editor") == 0,
                 "active node round trip");
    TEST_REQUIRE(decoded.editing, "editing state round trip");
    TEST_REQUIRE(!decoded.synchronising,
                 "synchronising state round trip");
    return 0;
}

static int test_heartbeat(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutPresenceStoreRepository repository;
    UmiWorkbenchLayoutPresence presence = make_presence(
        "presence-heartbeat", "layout.shared", "developer", "client-a",
        1000U, 1000U);
    UmiWorkbenchLayoutPresence loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_presence_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        &repository, &presence));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_heartbeat(
        &repository, &presence, "output", false, true, 1500U, 5000U));
    TEST_REQUIRE(strcmp(presence.active_node_id, "output") == 0,
                 "heartbeat updates node");
    TEST_REQUIRE(!presence.editing && presence.synchronising,
                 "heartbeat updates activity");
    TEST_REQUIRE(presence.expires_at_ms == 6500U,
                 "heartbeat extends expiry");
    TEST_STATUS_OK(umi_workbench_layout_presence_store_load(
        &repository, "layout.shared", "presence-heartbeat", 0U, &loaded));
    TEST_REQUIRE(strcmp(loaded.active_node_id, "output") == 0,
                 "heartbeat persisted");
    umi_data_server_destroy(server);
    return 0;
}

static int test_active_count_and_expiry(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutPresenceStoreRepository repository;
    UmiWorkbenchLayoutPresence first = make_presence(
        "presence-one", "layout.shared", "developer", "client-a",
        1000U, 1000U);
    UmiWorkbenchLayoutPresence second = make_presence(
        "presence-two", "layout.shared", "reviewer", "client-b",
        1000U, 5000U);
    UmiWorkbenchLayoutPresence third = make_presence(
        "presence-three", "layout.other", "analyst", "client-c",
        1000U, 5000U);
    size_t expired = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_presence_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        &repository, &second));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        &repository, &third));
    TEST_REQUIRE(umi_workbench_layout_presence_store_active_count(
        &repository, 1500U) == 3U, "all presence active");
    TEST_REQUIRE(umi_workbench_layout_presence_store_active_count(
        &repository, 2500U) == 2U, "expired presence excluded");
    TEST_STATUS_OK(umi_workbench_layout_presence_store_expire(
        &repository, 2500U, &expired));
    TEST_REQUIRE(expired == 1U, "one expired record removed");
    TEST_REQUIRE(umi_workbench_layout_presence_store_active_count(
        &repository, 2500U) == 2U, "active records remain");
    umi_data_server_destroy(server);
    return 0;
}

static int test_presence_listing(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutPresenceStoreRepository repository;
    UmiWorkbenchLayoutPresence first = make_presence(
        "presence-one", "layout.shared", "developer", "client-a",
        1000U, 5000U);
    UmiWorkbenchLayoutPresence second = make_presence(
        "presence-two", "layout.shared", "reviewer", "client-b",
        1000U, 5000U);
    UmiWorkbenchLayoutPresence results[4];
    size_t count = 0U;
    size_t total = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_presence_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        &repository, &first));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_save(
        &repository, &second));
    TEST_STATUS_OK(umi_workbench_layout_presence_store_list(
        &repository, "layout.shared", NULL, NULL,
        results, 4U, &count, &total));
    TEST_REQUIRE(count == 2U && total == 2U,
                 "layout presence listed");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_presence_codec() == 0, "presence codec");
    TEST_REQUIRE(test_heartbeat() == 0, "heartbeat");
    TEST_REQUIRE(test_active_count_and_expiry() == 0,
                 "active expiry");
    TEST_REQUIRE(test_presence_listing() == 0,
                 "presence listing");
    return 0;
}
