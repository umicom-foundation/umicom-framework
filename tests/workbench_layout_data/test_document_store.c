/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_document_store.c
 *
 * PURPOSE:
 *   Verify authoritative semantic layout document persistence, optimistic revisions, listing, integrity and deletion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_create_and_load(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutDocument *source = test_allocate_layout(
        "layout.development");
    UmiWorkbenchLayoutDocument loaded;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && source != NULL, "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        &store, source, 0U, 2000U, &revision));
    TEST_REQUIRE(revision == 1U, "first revision assigned");
    TEST_STATUS_OK(umi_workbench_layout_document_store_load(
        &store, "layout.development", &loaded));
    TEST_REQUIRE(strcmp(loaded.identity.layout_id,
        "layout.development") == 0, "layout identity restored");
    TEST_REQUIRE(loaded.node_count == source->node_count,
                 "semantic nodes restored");
    TEST_REQUIRE(loaded.version.revision == 1U,
                 "stored revision restored");
    TEST_REQUIRE(loaded.content_hash == source->content_hash,
                 "content hash restored");
    free(source);
    umi_data_server_destroy(server);
    return 0;
}

static int test_optimistic_update(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout(
        "layout.optimistic");
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL && document != NULL, "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        &store, document, 0U, 1000U, &revision));
    TEST_REQUIRE(revision == 1U, "initial revision");
    TEST_REQUIRE(umi_workbench_layout_document_store_save(
        &store, document, 0U, 2000U, &revision) ==
        UMI_STATUS_ALREADY_EXISTS, "create-only update rejected");
    TEST_REQUIRE(umi_workbench_layout_document_store_save(
        &store, document, 7U, 2000U, &revision) ==
        UMI_STATUS_INVALID_STATE, "stale revision rejected");
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document, "Optimistic Updated", "development",
        "Updated through the expected revision."));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        &store, document, 1U, 2000U, &revision));
    TEST_REQUIRE(revision == 2U, "second revision assigned");
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

static int test_list_and_query(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutDocument *first = test_allocate_layout("layout.first");
    UmiWorkbenchLayoutDocument *second = test_allocate_layout("layout.second");
    UmiWorkbenchLayoutStoreQuery query;
    UmiWorkbenchLayoutStoreList list;
    uint64_t revision;
    TEST_REQUIRE(server != NULL && first != NULL && second != NULL,
                 "create listing fixture");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        &store, first, 0U, 1000U, &revision));
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        second, "Operations Layout", "operations", "Operations panels."));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        &store, second, 0U, 2000U, &revision));
    (void)memset(&query, 0, sizeof(query));
    query.structure_size = sizeof(query);
    test_copy_text(query.owner_user_id,
                   sizeof(query.owner_user_id), "developer");
    test_copy_text(query.workspace_id,
                   sizeof(query.workspace_id), "workspace.framework");
    test_copy_text(query.category,
                   sizeof(query.category), "development");
    query.limit = UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    TEST_STATUS_OK(umi_workbench_layout_document_store_list(
        &store, &query, &list));
    TEST_REQUIRE(list.count == 1U, "category filters list");
    TEST_REQUIRE(strcmp(list.records[0].layout_id, "layout.first") == 0,
                 "expected layout listed");
    free(first);
    free(second);
    umi_data_server_destroy(server);
    return 0;
}

static int test_verify_and_delete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutDocument *document = test_allocate_layout("layout.delete");
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiWorkbenchLayoutDocument loaded;
    uint64_t revision;
    TEST_REQUIRE(server != NULL && document != NULL, "create fixture");
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_STATUS_OK(umi_workbench_layout_document_store_save(
        &store, document, 0U, 1000U, &revision));
    TEST_STATUS_OK(umi_workbench_layout_document_store_verify(
        &store, "layout.delete", &manifest));
    TEST_REQUIRE(manifest.revision == 1U, "manifest revision verified");
    TEST_REQUIRE(umi_workbench_layout_document_store_delete(
        &store, "layout.delete", 2U) == UMI_STATUS_INVALID_STATE,
        "wrong delete revision rejected");
    TEST_STATUS_OK(umi_workbench_layout_document_store_delete(
        &store, "layout.delete", 1U));
    TEST_REQUIRE(umi_workbench_layout_document_store_load(
        &store, "layout.delete", &loaded) == UMI_STATUS_NOT_FOUND,
        "deleted document absent");
    free(document);
    umi_data_server_destroy(server);
    return 0;
}

static int test_invalid_document_rejected(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutDocumentStore store;
    UmiWorkbenchLayoutDocument document;
    uint64_t revision = 0U;
    TEST_REQUIRE(server != NULL, "create server");
    umi_workbench_layout_document_init(
        &document, "layout.invalid", "Invalid");
    document.root_index = 0U;
    TEST_STATUS_OK(umi_workbench_layout_document_store_init(&store, server));
    TEST_REQUIRE(umi_workbench_layout_document_store_save(
        &store, &document, 0U, 1000U, &revision) != UMI_STATUS_OK,
        "invalid semantic document rejected");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_create_and_load() == 0, "create load");
    TEST_REQUIRE(test_optimistic_update() == 0, "optimistic update");
    TEST_REQUIRE(test_list_and_query() == 0, "list query");
    TEST_REQUIRE(test_verify_and_delete() == 0, "verify delete");
    TEST_REQUIRE(test_invalid_document_rejected() == 0,
                 "invalid document");
    return 0;
}
