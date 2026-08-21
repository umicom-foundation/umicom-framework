/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_revision_store.c
 *
 * PURPOSE:
 *   Verify immutable layout revision evidence, deterministic encoding and bounded history queries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static UmiWorkbenchLayoutRevisionRecord make_revision(
    const char *revision_id,
    const char *layout_id,
    uint64_t revision,
    uint64_t base_revision,
    bool deletion)
{
    UmiWorkbenchLayoutRevisionRecord record;
    (void)memset(&record, 0, sizeof(record));
    record.structure_size = sizeof(record);
    test_copy_text(record.revision_id,
                   sizeof(record.revision_id), revision_id);
    test_copy_text(record.layout_id, sizeof(record.layout_id), layout_id);
    test_copy_text(record.actor_id, sizeof(record.actor_id), "developer");
    test_copy_text(record.correlation_id,
                   sizeof(record.correlation_id), "correlation");
    record.revision = revision;
    record.base_revision = base_revision;
    record.content_hash = 1000U + revision;
    record.created_at_ms = 2000U + revision;
    record.deletion = deletion;
    test_copy_text(record.summary, sizeof(record.summary),
                   deletion ? "layout deleted" : "layout saved");
    return record;
}

static UmiStatus same_layout(
    const void *record_value,
    void *context,
    bool *out_matches)
{
    const UmiWorkbenchLayoutRevisionRecord *record =
        (const UmiWorkbenchLayoutRevisionRecord *)record_value;
    const char *layout_id = (const char *)context;
    if (record == NULL || layout_id == NULL || out_matches == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_matches = strcmp(record->layout_id, layout_id) == 0;
    return UMI_STATUS_OK;
}

static int test_revision_codec(void)
{
    UmiWorkbenchLayoutRevisionRecord source = make_revision(
        "revision-7", "layout.development", 7U, 6U, false);
    UmiWorkbenchLayoutRevisionRecord decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    TEST_STATUS_OK(umi_workbench_layout_revision_store_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_revision_store_decode(
        value, &decoded));
    TEST_REQUIRE(strcmp(decoded.revision_id, "revision-7") == 0,
                 "revision id round trip");
    TEST_REQUIRE(strcmp(decoded.layout_id, "layout.development") == 0,
                 "layout id round trip");
    TEST_REQUIRE(decoded.revision == 7U,
                 "revision number round trip");
    TEST_REQUIRE(decoded.base_revision == 6U,
                 "base revision round trip");
    TEST_REQUIRE(!decoded.deletion, "deletion state round trip");
    return 0;
}

static int test_revision_history(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutRevisionStoreRepository repository;
    UmiWorkbenchLayoutRevisionRecord records[4];
    UmiWorkbenchLayoutRevisionRecord results[4];
    size_t count = 0U;
    size_t total = 0U;
    size_t index;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_revision_store_repository_init(
        &repository, server));
    records[0] = make_revision("rev-a1", "layout.a", 1U, 0U, false);
    records[1] = make_revision("rev-a2", "layout.a", 2U, 1U, false);
    records[2] = make_revision("rev-b1", "layout.b", 1U, 0U, false);
    records[3] = make_revision("rev-a3", "layout.a", 3U, 2U, true);
    for (index = 0U; index < 4U; ++index) {
        TEST_STATUS_OK(umi_workbench_layout_revision_store_save(
            &repository, &records[index]));
    }
    TEST_STATUS_OK(umi_workbench_layout_revision_store_list(
        &repository, NULL, same_layout, "layout.a",
        results, 4U, &count, &total));
    TEST_REQUIRE(count == 3U && total == 3U,
                 "layout history filtered");
    TEST_REQUIRE(results[0].revision <= results[count - 1U].revision,
                 "history follows sequence order");
    TEST_REQUIRE(results[count - 1U].deletion,
                 "deletion revision retained");
    umi_data_server_destroy(server);
    return 0;
}

static int test_revision_load_delete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutRevisionStoreRepository repository;
    UmiWorkbenchLayoutRevisionRecord source = make_revision(
        "revision-9", "layout.load", 9U, 8U, false);
    UmiWorkbenchLayoutRevisionRecord loaded;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_revision_store_repository_init(
        &repository, server));
    TEST_STATUS_OK(umi_workbench_layout_revision_store_save(
        &repository, &source));
    TEST_STATUS_OK(umi_workbench_layout_revision_store_load(
        &repository, NULL, "revision-9", 9U, &loaded));
    TEST_REQUIRE(loaded.content_hash == source.content_hash,
                 "revision evidence restored");
    TEST_STATUS_OK(umi_workbench_layout_revision_store_delete(
        &repository, NULL, "revision-9", 9U));
    TEST_REQUIRE(umi_workbench_layout_revision_store_load(
        &repository, NULL, "revision-9", 9U, &loaded) ==
        UMI_STATUS_NOT_FOUND, "deleted revision absent");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_revision_codec() == 0, "revision codec");
    TEST_REQUIRE(test_revision_history() == 0, "revision history");
    TEST_REQUIRE(test_revision_load_delete() == 0,
                 "revision load delete");
    return 0;
}
