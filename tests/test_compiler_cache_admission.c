/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_cache_admission.c
 *
 * PURPOSE:
 *   Verify cache owner admission, checked invalidation, replacement and saturating read statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/compiler/cache.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"cache admission line %d\n",__LINE__); return 1; } } while (0)
static UmiCompilerCache cache, before;
static UmiCompilerCacheEntry item;
static void reset_cache(void)
{
    (void)memset(&cache,0,sizeof(cache)); (void)memset(&item,0,sizeof(item));
    item.key = 1U; item.created_at_ns = 5U; item.valid = true;
    (void)strcpy(item.artifact_path,"build/object.o");
}
int main(void)
{
    reset_cache();
    REQUIRE(umi_compiler_cache_valid(&cache));
    REQUIRE(!umi_compiler_cache_valid(NULL));
    REQUIRE(umi_compiler_cache_invalidate_checked(NULL,0U) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_OK);
    REQUIRE(umi_compiler_cache_get(&cache,1U) == &cache.items[0] && cache.hits == 1U);
    REQUIRE(umi_compiler_cache_get(&cache,2U) == NULL && cache.misses == 1U);
    REQUIRE(umi_compiler_cache_invalidate_checked(&cache,5U) == UMI_STATUS_OK);
    REQUIRE(cache.items[0].valid && cache.revision == 2U);
    umi_compiler_cache_invalidate(&cache,6U);
    REQUIRE(!cache.items[0].valid && cache.revision == 3U);
    REQUIRE(umi_compiler_cache_get(&cache,1U) == NULL && cache.misses == 2U);
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_OK && cache.count == 1U);
    cache.hits = cache.misses = cache.revision = UINT64_MAX;
    REQUIRE(umi_compiler_cache_get(&cache,1U) != NULL);
    REQUIRE(umi_compiler_cache_get(&cache,2U) == NULL);
    REQUIRE(cache.hits == UINT64_MAX && cache.misses == UINT64_MAX);
    before = cache;
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(umi_compiler_cache_invalidate_checked(&cache,6U) == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_compiler_cache_invalidate(&cache,6U);
    REQUIRE(memcmp(&cache,&before,sizeof(cache)) == 0);
    reset_cache();
    for (unsigned field = 0U; field < 3U; ++field) {
        reset_cache(); before = cache;
        if (field == 0U) item.key = 0U;
        else if (field == 1U) item.artifact_path[0] = '\0';
        else (void)memset(item.artifact_path,'x',sizeof(item.artifact_path));
        REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&cache,&before,sizeof(cache)) == 0);
    }
    reset_cache();
    (void)memset(item.artifact_path,'x',sizeof(item.artifact_path)-1U);
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_OK);
    /* Invalidated rows also own text. They cannot hide damaged storage. */
    cache.items[1] = item; cache.items[1].key = 2U; cache.items[1].valid = false; cache.count = 2U;
    (void)memset(cache.items[1].artifact_path,'x',sizeof(cache.items[1].artifact_path)); before = cache;
    REQUIRE(umi_compiler_cache_get(&cache,1U) == NULL);
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_INVALID_STATE);
    REQUIRE(umi_compiler_cache_invalidate_checked(&cache,6U) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(&cache,&before,sizeof(cache)) == 0);
    for (unsigned oversized = 0U; oversized < 2U; ++oversized) {
        reset_cache(); cache.count = oversized ? SIZE_MAX : UMI_COMPILER_MAX_CACHE_ENTRIES+1U; before = cache;
        REQUIRE(umi_compiler_cache_get(&cache,1U) == NULL);
        REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_compiler_cache_invalidate_checked(&cache,6U) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&cache,&before,sizeof(cache)) == 0);
    }
    reset_cache();
    for (size_t index = 0U; index < UMI_COMPILER_MAX_CACHE_ENTRIES; ++index) {
        item.key = (uint64_t)index + 1U;
        REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_OK);
    }
    /* Full capacity permits replacement, not insertion of a different key. */
    (void)strcpy(item.artifact_path,"replacement.o");
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_OK);
    REQUIRE(strcmp(cache.items[cache.count-1U].artifact_path,"replacement.o") == 0);
    before = cache; item.key += 1U;
    REQUIRE(umi_compiler_cache_put(&cache,&item) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(&cache,&before,sizeof(cache)) == 0);
    reset_cache(); cache.items[0] = item;
    REQUIRE(umi_compiler_cache_put(&cache,&cache.items[0]) == UMI_STATUS_OK);
    REQUIRE(umi_compiler_cache_put(&cache,&cache.items[0]) == UMI_STATUS_OK);
    REQUIRE(cache.count == 1U && cache.revision == 2U);
    before = cache;
    REQUIRE(umi_compiler_cache_get(&cache,0U) == NULL);
    REQUIRE(memcmp(&cache,&before,sizeof(cache)) == 0);
    reset_cache();
    REQUIRE(umi_compiler_cache_invalidate_checked(&cache,0U) == UMI_STATUS_OK && cache.revision == 1U);
    return 0;
}
