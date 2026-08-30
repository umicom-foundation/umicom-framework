/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/cache.c
 *
 * PURPOSE:
 *   Implement the cache behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler cache metadata | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/cache.h"
#include <string.h>
static uint64_t request_hash_bytes(uint64_t hash,const void *data,size_t count) { const unsigned char *bytes = (const unsigned char *)data; size_t index; for (index = 0U; index < count; ++index) { hash ^= bytes[index]; hash *= UINT64_C(1099511628211); } return hash; }
static uint64_t request_hash_string(uint64_t hash,const char *text) { return request_hash_bytes(hash,text,strlen(text) + 1U); }
uint64_t umi_compiler_request_hash(const UmiCompilerRequest *request)
{
    uint64_t hash = UINT64_C(1469598103934665603); size_t index; if (request == NULL) return 0U;
    hash = request_hash_string(hash,request->project_id); hash = request_hash_string(hash,request->unit_id); hash = request_hash_string(hash,request->source_path); hash = request_hash_string(hash,request->output_path); hash = request_hash_string(hash,request->standard); hash = request_hash_bytes(hash,&request->language,sizeof(request->language)); hash = request_hash_bytes(hash,&request->action,sizeof(request->action)); hash = request_hash_string(hash,request->target.triple);
    for (index = 0U; index < request->include_directory_count; ++index) {
        hash = request_hash_string(hash,request->include_directories[index]);
    }
    for (index = 0U; index < request->definition_count; ++index) {
        hash = request_hash_string(hash,request->definitions[index]);
    }
    for (index = 0U; index < request->option_count; ++index) {
        hash = request_hash_string(hash,request->options[index]);
    }
    return hash;
}
UmiStatus umi_compiler_cache_put(UmiCompilerCache *cache,const UmiCompilerCacheEntry *entry)
{ size_t index; if (cache == NULL || entry == NULL || entry->key == 0U || entry->artifact_path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < cache->count; ++index) if (cache->items[index].key == entry->key) { cache->items[index] = *entry; cache->revision += 1U; return UMI_STATUS_OK; } if (cache->count >= UMI_COMPILER_MAX_CACHE_ENTRIES) return UMI_STATUS_CAPACITY_EXCEEDED; cache->items[cache->count++] = *entry; cache->revision += 1U; return UMI_STATUS_OK; }
const UmiCompilerCacheEntry *umi_compiler_cache_get(UmiCompilerCache *cache,uint64_t key)
{ size_t index; if (cache == NULL || key == 0U) return NULL; for (index = 0U; index < cache->count; ++index) if (cache->items[index].key == key && cache->items[index].valid) { cache->hits += 1U; return &cache->items[index]; } cache->misses += 1U; return NULL; }
void umi_compiler_cache_invalidate(UmiCompilerCache *cache,uint64_t older_than_ns)
{ size_t index; if (cache == NULL) return; for (index = 0U; index < cache->count; ++index) if (cache->items[index].created_at_ns < older_than_ns) cache->items[index].valid = false; cache->revision += 1U; }
