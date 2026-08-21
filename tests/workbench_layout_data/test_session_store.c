/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_session_store.c
 *
 * PURPOSE:
 *   Verify workbench session serialisation, optimistic checkpoints, open-panel recovery evidence and deletion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_session_codec(void)
{
    UmiWorkbenchLayoutSession source = test_create_session(
        "session.codec", "layout.development");
    UmiWorkbenchLayoutSession decoded;
    char *text = NULL;
    size_t size = 0U;
    TEST_STATUS_OK(umi_workbench_layout_session_encode(
        &source, &text, &size));
    TEST_REQUIRE(text != NULL && size == strlen(text),
                 "session encoded");
    TEST_STATUS_OK(umi_workbench_layout_session_decode(text, &decoded));
    TEST_REQUIRE(strcmp(decoded.session_id, "session.codec") == 0,
                 "session identity decoded");
    TEST_REQUIRE(strcmp(decoded.active_layout_id,
        "layout.development") == 0, "active layout decoded");
    TEST_REQUIRE(decoded.panel_count == 1U,
                 "open panel decoded");
    TEST_REQUIRE(decoded.recovery_available,
                 "recovery evidence decoded");
    free(text);
    return 0;
}

static int test_checkpoint_and_load(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSessionStore store;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.main", "layout.development");
    UmiWorkbenchLayoutSession loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_session_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_session_store_save(
        &store, &session, 0U, &revision));
    TEST_REQUIRE(revision == 1U, "first checkpoint revision");
    TEST_STATUS_OK(umi_workbench_layout_session_store_load(
        &store, "session.main", &loaded));
    TEST_REQUIRE(loaded.revision == 1U, "revision restored");
    TEST_REQUIRE(!loaded.clean_shutdown, "unclean state retained");
    TEST_REQUIRE(strcmp(loaded.panels[0].node_id, "project") == 0,
                 "panel placement retained");
    umi_data_server_destroy(server);
    return 0;
}

static int test_session_update_conflict(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSessionStore store;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.conflict", "layout.development");
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_session_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_session_store_save(
        &store, &session, 0U, &revision));
    TEST_REQUIRE(umi_workbench_layout_session_store_save(
        &store, &session, 9U, &revision) == UMI_STATUS_INVALID_STATE,
        "stale checkpoint rejected");
    session.clean_shutdown = true;
    session.recovery_available = false;
    session.last_checkpoint_at_ms = 2000U;
    TEST_STATUS_OK(umi_workbench_layout_session_store_save(
        &store, &session, 1U, &revision));
    TEST_REQUIRE(revision == 2U, "updated checkpoint revision");
    umi_data_server_destroy(server);
    return 0;
}

static int test_session_delete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutSessionStore store;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.delete", "layout.development");
    UmiWorkbenchLayoutSession loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_session_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_session_store_save(
        &store, &session, 0U, &revision));
    TEST_REQUIRE(umi_workbench_layout_session_store_delete(
        &store, "session.delete", 7U) == UMI_STATUS_INVALID_STATE,
        "stale delete rejected");
    TEST_STATUS_OK(umi_workbench_layout_session_store_delete(
        &store, "session.delete", revision));
    TEST_REQUIRE(umi_workbench_layout_session_store_load(
        &store, "session.delete", &loaded) == UMI_STATUS_NOT_FOUND,
        "deleted session absent");
    umi_data_server_destroy(server);
    return 0;
}

static int test_malformed_session(void)
{
    UmiWorkbenchLayoutSession session;
    TEST_REQUIRE(umi_workbench_layout_session_decode(
        "schema=unknown\\n", &session) == UMI_STATUS_PARSE_ERROR,
        "unknown session schema rejected");
    TEST_REQUIRE(umi_workbench_layout_session_decode(
        NULL, &session) == UMI_STATUS_INVALID_ARGUMENT,
        "null session text rejected");
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_session_codec() == 0, "session codec");
    TEST_REQUIRE(test_checkpoint_and_load() == 0, "checkpoint load");
    TEST_REQUIRE(test_session_update_conflict() == 0, "update conflict");
    TEST_REQUIRE(test_session_delete() == 0, "session delete");
    TEST_REQUIRE(test_malformed_session() == 0, "malformed session");
    return 0;
}
