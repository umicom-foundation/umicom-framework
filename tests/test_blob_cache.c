/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_blob_cache.c
 *
 * PURPOSE:
 *   Verify binary blob round-trip and LRU cache eviction statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataServer *server = NULL;
    UmiStore store;
    UmiBlobStore blobs;
    UmiCache *cache = NULL;
    unsigned char input[4] = {0U, 1U, 2U, 255U};
    unsigned char *output = NULL;
    size_t output_size = 0U;
    char value[32];
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_store_from_data_server(server, &store) == UMI_STATUS_OK);
    assert(umi_blob_store_init(&blobs, &store, "test") == UMI_STATUS_OK);
    assert(umi_blob_store_put(&blobs, "blob", input, sizeof(input)) == UMI_STATUS_OK);
    assert(umi_blob_store_get(&blobs, "blob", &output, &output_size) == UMI_STATUS_OK);
    assert(output_size == sizeof(input) && memcmp(output, input, sizeof(input)) == 0);
    umi_blob_store_free(output);
    assert(umi_cache_create(1U, &cache) == UMI_STATUS_OK);
    assert(umi_cache_put(cache, "a", "one") == UMI_STATUS_OK);
    assert(umi_cache_get(cache, "a", value, sizeof(value)) == UMI_STATUS_OK);
    assert(umi_cache_put(cache, "b", "two") == UMI_STATUS_OK);
    assert(umi_cache_stats(cache).evictions == 1U);
    umi_cache_destroy(cache);
    umi_data_server_destroy(server);
    return 0;
}
