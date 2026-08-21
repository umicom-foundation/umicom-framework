/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_data_server_store.c
 *
 * PURPOSE:
 *   Verify the Data Server implementation of the existing Framework layout store port and transaction lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_adapter_layout_lifecycle(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutDocument *source = test_allocate_layout("layout.adapter");
    UmiWorkbenchLayoutDocument loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && source != NULL, "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_init(
        &store, server));
    adapter = umi_workbench_layout_data_server_store_adapter(&store);
    TEST_REQUIRE(adapter.structure_size == sizeof(adapter),
                 "adapter structure size");
    TEST_REQUIRE(adapter.save_layout != NULL && adapter.load_layout != NULL,
                 "layout functions available");
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context, source, 0U, &revision));
    TEST_REQUIRE(revision == 1U, "adapter save revision");
    TEST_STATUS_OK(adapter.load_layout(
        adapter.context, "layout.adapter", &loaded));
    TEST_REQUIRE(strcmp(loaded.identity.layout_id, "layout.adapter") == 0,
                 "adapter load identity");
    TEST_STATUS_OK(adapter.delete_layout(
        adapter.context, "layout.adapter", revision));
    free(source);
    umi_data_server_destroy(server);
    return 0;
}

static int test_adapter_session_lifecycle(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutSession session = test_create_session(
        "session.adapter", "layout.adapter");
    UmiWorkbenchLayoutSession loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_init(
        &store, server));
    adapter = umi_workbench_layout_data_server_store_adapter(&store);
    TEST_STATUS_OK(adapter.save_session(
        adapter.context, &session, 0U, &revision));
    TEST_REQUIRE(revision == 1U, "session revision");
    TEST_STATUS_OK(adapter.load_session(
        adapter.context, "session.adapter", &loaded));
    TEST_REQUIRE(strcmp(loaded.active_layout_id, "layout.adapter") == 0,
                 "active layout retained");
    TEST_STATUS_OK(adapter.delete_session(
        adapter.context, "session.adapter", revision));
    umi_data_server_destroy(server);
    return 0;
}

static int test_adapter_transactions(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.transaction");
    UmiWorkbenchLayoutDocument loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL, "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_init(
        &store, server));
    adapter = umi_workbench_layout_data_server_store_adapter(&store);
    TEST_STATUS_OK(adapter.begin_transaction(adapter.context));
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context, document, 0U, &revision));
    TEST_STATUS_OK(adapter.rollback_transaction(adapter.context));
    TEST_REQUIRE(adapter.load_layout(
        adapter.context, "layout.transaction", &loaded) ==
        UMI_STATUS_NOT_FOUND, "rollback removes provisional save");

    TEST_STATUS_OK(adapter.begin_transaction(adapter.context));
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context, document, 0U, &revision));
    TEST_STATUS_OK(adapter.commit_transaction(adapter.context));
    TEST_STATUS_OK(adapter.load_layout(
        adapter.context, "layout.transaction", &loaded));
    TEST_REQUIRE(loaded.version.revision == 1U,
                 "committed layout remains");
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

static int test_adapter_verify(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.verify");
    uint64_t revision = 0U;
    size_t layout_count = 0U;
    size_t session_count = 0U;
    TEST_REQUIRE(server != NULL && document != NULL, "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_init(
        &store, server));
    adapter = umi_workbench_layout_data_server_store_adapter(&store);
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context, document, 0U, &revision));
    TEST_STATUS_OK(umi_workbench_layout_data_server_store_verify(
        &store, &layout_count, &session_count));
    TEST_REQUIRE(layout_count == 1U && session_count == 0U,
                 "store verification reports authoritative counts");
    TEST_REQUIRE(umi_workbench_layout_data_server_store_revision(&store) > 0U,
                 "store revision advances");
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

static int test_invalid_adapter_creation(void)
{
    UmiWorkbenchLayoutDataServerStore store;
    TEST_REQUIRE(umi_workbench_layout_data_server_store_init(
        NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT,
        "null store rejected");
    TEST_REQUIRE(umi_workbench_layout_data_server_store_init(
        &store, NULL) == UMI_STATUS_INVALID_ARGUMENT,
        "null server rejected");
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_adapter_layout_lifecycle() == 0,
                 "adapter layout lifecycle");
    TEST_REQUIRE(test_adapter_session_lifecycle() == 0,
                 "adapter session lifecycle");
    TEST_REQUIRE(test_adapter_transactions() == 0,
                 "adapter transactions");
    TEST_REQUIRE(test_adapter_verify() == 0, "adapter verify");
    TEST_REQUIRE(test_invalid_adapter_creation() == 0,
                 "invalid adapter");
    return 0;
}
