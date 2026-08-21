/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_chunk_store.c
 *
 * PURPOSE:
 *   Verify transactional chunked storage, integrity checking, stale-chunk removal and bounded payload recovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static char *make_payload(size_t length)
{
    char *payload = (char *)malloc(length + 1U);
    size_t index;
    if (payload == NULL) return NULL;
    for (index = 0U; index < length; ++index) {
        payload[index] = (char)('a' + (int)(index % 26U));
    }
    payload[length] = '\0';
    return payload;
}

static int test_manifest_codec(void)
{
    UmiWorkbenchLayoutDataChunkManifest source;
    UmiWorkbenchLayoutDataChunkManifest decoded;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    (void)memset(&source, 0, sizeof(source));
    source.structure_size = sizeof(source);
    source.manifest_kind = UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST;
    source.chunk_kind = UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK;
    source.chunk_count = 3U;
    source.byte_count = 7201U;
    source.content_hash = 123456U;
    source.revision = 9U;
    source.modified_at_ms = 2000U;
    test_copy_text(source.aggregate_id, sizeof(source.aggregate_id),
                   "layout.chunked");
    TEST_STATUS_OK(umi_workbench_layout_chunk_manifest_encode(
        &source, value, sizeof(value), &required));
    TEST_STATUS_OK(umi_workbench_layout_chunk_manifest_decode(
        value, &decoded));
    TEST_REQUIRE(strcmp(decoded.aggregate_id, "layout.chunked") == 0,
                 "manifest identity");
    TEST_REQUIRE(decoded.chunk_count == 3U, "manifest chunk count");
    TEST_REQUIRE(decoded.byte_count == 7201U, "manifest byte count");
    TEST_REQUIRE(decoded.revision == 9U, "manifest revision");
    return 0;
}

static int test_multi_chunk_round_trip(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChunkStore store;
    UmiWorkbenchLayoutDataChunkManifest saved;
    UmiWorkbenchLayoutDataChunkManifest loaded_manifest;
    char *payload = make_payload(9001U);
    char *loaded = NULL;
    TEST_REQUIRE(server != NULL && payload != NULL, "allocate fixture");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_init(
        &store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK));
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_save(
        &store, "layout.chunked", payload, 1U, 1000U, &saved));
    TEST_REQUIRE(saved.chunk_count >= 3U, "payload split into chunks");
    TEST_REQUIRE(saved.byte_count == 9001U, "payload size retained");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_verify(
        &store, "layout.chunked", &loaded_manifest));
    TEST_REQUIRE(loaded_manifest.content_hash == saved.content_hash,
                 "integrity hash retained");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_load(
        &store, "layout.chunked", &loaded, &loaded_manifest));
    TEST_REQUIRE(strcmp(loaded, payload) == 0, "payload round trip");
    TEST_REQUIRE(loaded_manifest.revision == 1U, "revision round trip");
    free(loaded);
    free(payload);
    umi_data_server_destroy(server);
    return 0;
}

static int test_rewrite_removes_stale_chunks(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChunkStore store;
    UmiWorkbenchLayoutDataChunkManifest first;
    UmiWorkbenchLayoutDataChunkManifest second;
    char *large = make_payload(10000U);
    char *loaded = NULL;
    char stale_key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char value[32];
    TEST_REQUIRE(server != NULL && large != NULL, "allocate rewrite fixture");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_init(
        &store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK));
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_save(
        &store, "layout.rewrite", large, 1U, 1000U, &first));
    TEST_REQUIRE(first.chunk_count > 1U, "first save uses multiple chunks");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_save(
        &store, "layout.rewrite", "small", 2U, 2000U, &second));
    TEST_REQUIRE(second.chunk_count == 1U, "rewrite uses one chunk");
    TEST_STATUS_OK(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK,
        "layout.rewrite", NULL, 0U, 1U,
        stale_key, sizeof(stale_key)));
    TEST_REQUIRE(umi_data_server_get(server, stale_key,
        value, sizeof(value)) == UMI_STATUS_NOT_FOUND,
        "stale chunk removed");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_load(
        &store, "layout.rewrite", &loaded, NULL));
    TEST_REQUIRE(strcmp(loaded, "small") == 0, "rewrite recovered");
    free(loaded);
    free(large);
    umi_data_server_destroy(server);
    return 0;
}

static int test_integrity_failure(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChunkStore store;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *loaded = NULL;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_init(
        &store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK));
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_save(
        &store, "layout.integrity", "authoritative", 1U, 1000U, NULL));
    TEST_STATUS_OK(umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK,
        "layout.integrity", NULL, 0U, 0U, key, sizeof(key)));
    TEST_STATUS_OK(umi_data_server_set(server, key, "tampered"));
    TEST_REQUIRE(umi_workbench_layout_chunk_store_load(
        &store, "layout.integrity", &loaded, NULL) ==
        UMI_STATUS_PARSE_ERROR, "tampering detected");
    TEST_REQUIRE(loaded == NULL, "failed load returns no payload");
    umi_data_server_destroy(server);
    return 0;
}

static int test_delete(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiWorkbenchLayoutChunkStore store;
    char *loaded = NULL;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_init(
        &store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK));
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_save(
        &store, "session.main", "session-state", 1U, 1000U, NULL));
    TEST_STATUS_OK(umi_workbench_layout_chunk_store_delete(
        &store, "session.main"));
    TEST_REQUIRE(umi_workbench_layout_chunk_store_load(
        &store, "session.main", &loaded, NULL) ==
        UMI_STATUS_NOT_FOUND, "deleted payload absent");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_manifest_codec() == 0, "manifest codec");
    TEST_REQUIRE(test_multi_chunk_round_trip() == 0, "multi chunk");
    TEST_REQUIRE(test_rewrite_removes_stale_chunks() == 0,
                 "rewrite stale chunks");
    TEST_REQUIRE(test_integrity_failure() == 0, "integrity failure");
    TEST_REQUIRE(test_delete() == 0, "delete");
    return 0;
}
