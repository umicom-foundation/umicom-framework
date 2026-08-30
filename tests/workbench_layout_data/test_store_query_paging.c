/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_store_query_paging.c
 *
 * PURPOSE:
 *   Verify Data Server layout browser queries, deterministic offsets, limits, text search and total-result evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int save_layout(
    UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    const char *name,
    const char *category,
    uint64_t modified_at_ms)
{
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(layout_id);
    uint64_t revision = 0U;
    if (document == NULL) return 1;
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document, name, category, "Queryable layout record."));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        store, document, 0U, modified_at_ms, &revision));
    free(document);
    return 0;
}

static int test_offset_and_limit(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutStoreQuery query;
    UmiWorkbenchLayoutStoreList list;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_REQUIRE(save_layout(&store, "layout.a", "Alpha", "development", 1000U) == 0,
                 "save alpha");
    TEST_REQUIRE(save_layout(&store, "layout.b", "Beta", "development", 2000U) == 0,
                 "save beta");
    TEST_REQUIRE(save_layout(&store, "layout.c", "Gamma", "development", 3000U) == 0,
                 "save gamma");
    query = umi_workbench_layout_store_query_default();
    query.offset = 1U;
    query.limit = 1U;
    TEST_STATUS_OK(umi_workbench_layout_document_store_list(
        &store, &query, &list));
    TEST_REQUIRE(list.count == 1U, "one result returned");
    TEST_REQUIRE(list.total_available == 3U,
                 "total available retained");
    TEST_REQUIRE(list.truncated, "paging reports truncation");
    umi_data_server_destroy(server);
    return 0;
}

static int test_text_and_category_filter(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutStoreQuery query;
    UmiWorkbenchLayoutStoreList list;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_REQUIRE(save_layout(&store, "layout.development", "Development Workbench", "development", 1000U) == 0,
                 "save development");
    TEST_REQUIRE(save_layout(&store, "layout.operations", "Operations Centre", "operations", 2000U) == 0,
                 "save operations");
    query = umi_workbench_layout_store_query_default();
    test_copy_text(query.category, sizeof(query.category), "operations");
    test_copy_text(query.text, sizeof(query.text), "Centre");
    TEST_STATUS_OK(umi_workbench_layout_document_store_list(
        &store, &query, &list));
    TEST_REQUIRE(list.count == 1U,
                 "category and text filters combine");
    TEST_REQUIRE(strcmp(list.records[0].layout_id,
        "layout.operations") == 0,
        "matching layout returned");
    umi_data_server_destroy(server);
    return 0;
}

static int test_workspace_filter(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutStoreQuery query;
    UmiWorkbenchLayoutStoreList list;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_REQUIRE(save_layout(&store, "layout.workspace", "Workspace", "development", 1000U) == 0,
                 "save workspace layout");
    query = umi_workbench_layout_store_query_default();
    test_copy_text(query.workspace_id,
                   sizeof(query.workspace_id), "workspace.framework");
    TEST_STATUS_OK(umi_workbench_layout_document_store_list(
        &store, &query, &list));
    TEST_REQUIRE(list.count == 1U, "workspace match");
    test_copy_text(query.workspace_id,
                   sizeof(query.workspace_id), "workspace.other");
    TEST_STATUS_OK(umi_workbench_layout_document_store_list(
        &store, &query, &list));
    TEST_REQUIRE(list.count == 0U, "workspace mismatch");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_offset_and_limit() == 0, "offset limit");
    TEST_REQUIRE(test_text_and_category_filter() == 0,
                 "text category");
    TEST_REQUIRE(test_workspace_filter() == 0,
                 "workspace filter");
    return 0;
}
