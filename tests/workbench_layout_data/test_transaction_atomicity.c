/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_transaction_atomicity.c
 *
 * PURPOSE:
 *   Verify that nested Framework persistence operations participate in caller-owned Data Server transactions and roll back atomically.
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
 * Exercise test layout and session rollback and return a clear result when the behaviour
 * no longer matches its contract.
 */
static int test_layout_and_session_rollback(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.atomic");
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.atomic", "layout.atomic");
    UmiWorkbenchLayoutDocument loaded_document;
    UmiWorkbenchLayoutSession loaded_session;
    uint64_t layout_revision = 0U;
    uint64_t session_revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_init(
        &store, server));
    adapter = umi_workbench_layout_data_server_store_adapter(&store);
    TEST_STATUS_OK(adapter.begin_transaction(adapter.context));
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context, document, 0U, &layout_revision));
    TEST_STATUS_OK(adapter.save_session(
        adapter.context, &session, 0U, &session_revision));
    TEST_STATUS_OK(adapter.rollback_transaction(adapter.context));
    TEST_REQUIRE(adapter.load_layout(
        adapter.context, "layout.atomic", &loaded_document) ==
        UMI_STATUS_NOT_FOUND, "layout rolled back");
    TEST_REQUIRE(adapter.load_session(
        adapter.context, "session.atomic", &loaded_session) ==
        UMI_STATUS_NOT_FOUND, "session rolled back");
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test layout and session commit and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_layout_and_session_commit(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.atomic");
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.atomic", "layout.atomic");
    UmiWorkbenchLayoutDocument loaded_document;
    UmiWorkbenchLayoutSession loaded_session;
    uint64_t layout_revision = 0U;
    uint64_t session_revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL,
                 "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_init(
        &store, server));
    adapter = umi_workbench_layout_data_server_store_adapter(&store);
    TEST_STATUS_OK(adapter.begin_transaction(adapter.context));
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context, document, 0U, &layout_revision));
    TEST_STATUS_OK(adapter.save_session(
        adapter.context, &session, 0U, &session_revision));
    TEST_STATUS_OK(adapter.commit_transaction(adapter.context));
    TEST_STATUS_OK(adapter.load_layout(
        adapter.context, "layout.atomic", &loaded_document));
    TEST_STATUS_OK(adapter.load_session(
        adapter.context, "session.atomic", &loaded_session));
    TEST_REQUIRE(loaded_document.version.revision == 1U,
                 "layout committed");
    TEST_REQUIRE(loaded_session.revision == 1U,
                 "session committed");
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test nested begin rejected and return a clear result when the behaviour no
 * longer matches its contract.
 */
static int test_nested_begin_rejected(void)
{
    UmiDataServer *server = test_create_data_server();
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_begin(server));
    TEST_REQUIRE(umi_data_server_begin(server) == UMI_STATUS_BUSY,
                 "nested transaction rejected");
    TEST_STATUS_OK(umi_data_server_rollback(server));
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Exercise test commit without transaction rejected and return a clear result when the
 * behaviour no longer matches its contract.
 */
static int test_commit_without_transaction_rejected(void)
{
    UmiDataServer *server = test_create_data_server();
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(umi_data_server_commit(server) ==
        UMI_STATUS_INVALID_STATE,
        "commit without transaction rejected");
    TEST_REQUIRE(umi_data_server_rollback(server) ==
        UMI_STATUS_INVALID_STATE,
        "rollback without transaction rejected");
    umi_data_server_destroy(server);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    TEST_REQUIRE(test_layout_and_session_rollback() == 0,
                 "atomic rollback");
    TEST_REQUIRE(test_layout_and_session_commit() == 0,
                 "atomic commit");
    TEST_REQUIRE(test_nested_begin_rejected() == 0,
                 "nested begin");
    TEST_REQUIRE(test_commit_without_transaction_rejected() == 0,
                 "missing transaction");
    return 0;
}
