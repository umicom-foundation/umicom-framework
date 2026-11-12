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
#include "umicom/compiler/cache.h"
#include <string.h>
#include "catalogue_validation.h"

/* Complete owner admission and explicit publication limits replace unchecked traversal. The original operations are retained for review of catalogue, cache replacement and invalidation behavior. The previous implementation is retained for review. */
#if 0
/*
 * Provide the request hash bytes operation used by this module and its client
 * applications.
 */
static uint64_t request_hash_bytes(uint64_t hash,const void *data,size_t count) { const unsigned char *bytes = (const unsigned char *)data; size_t index; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < count; ++index) { hash ^= bytes[index]; hash *= UINT64_C(1099511628211); } return hash; }
/*
 * Provide the request hash string operation used by this module and its client
 * applications.
 */
static uint64_t request_hash_string(uint64_t hash,const char *text) { return request_hash_bytes(hash,text,strlen(text) + 1U); }
/*
 * Provide the compiler request hash operation used by this module and its client
 * applications.
 */
uint64_t umi_compiler_request_hash(const UmiCompilerRequest *request)
{
    uint64_t hash = UINT64_C(1469598103934665603); size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (request == NULL) return 0U;
    hash = request_hash_string(hash,request->project_id); hash = request_hash_string(hash,request->unit_id); hash = request_hash_string(hash,request->source_path); hash = request_hash_string(hash,request->output_path); hash = request_hash_string(hash,request->standard); hash = request_hash_bytes(hash,&request->language,sizeof(request->language)); hash = request_hash_bytes(hash,&request->action,sizeof(request->action)); hash = request_hash_string(hash,request->target.triple);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->include_directory_count; ++index) {
        hash = request_hash_string(hash,request->include_directories[index]);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->definition_count; ++index) {
        hash = request_hash_string(hash,request->definitions[index]);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->option_count; ++index) {
        hash = request_hash_string(hash,request->options[index]);
    }
    return hash;
}
/*
 * Provide the compiler cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_cache_put(UmiCompilerCache *cache,const UmiCompilerCacheEntry *entry)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache == NULL || entry == NULL || entry->key == 0U || entry->artifact_path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < cache->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache->items[index].key == entry->key) { cache->items[index] = *entry; cache->revision += 1U; return UMI_STATUS_OK; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache->count >= UMI_COMPILER_MAX_CACHE_ENTRIES) return UMI_STATUS_CAPACITY_EXCEEDED; cache->items[cache->count++] = *entry; cache->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the compiler cache get operation used by this module and its client
 * applications.
 */
const UmiCompilerCacheEntry *umi_compiler_cache_get(UmiCompilerCache *cache,uint64_t key)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache == NULL || key == 0U) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < cache->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache->items[index].key == key && cache->items[index].valid) { cache->hits += 1U; return &cache->items[index]; } cache->misses += 1U; return NULL; }
/*
 * Provide the compiler cache invalidate operation used by this module and its client
 * applications.
 */
void umi_compiler_cache_invalidate(UmiCompilerCache *cache,uint64_t older_than_ns)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache == NULL) return; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < cache->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (cache->items[index].created_at_ns < older_than_ns) cache->items[index].valid = false; cache->revision += 1U; }
#endif
/* This is a non-cryptographic identity of request metadata. It does not inspect
 * source files, dependency contents, driver versions, environment or artifacts. */
static uint64_t request_hash_bytes(uint64_t hash,const void *data,size_t count)
{
    const unsigned char *bytes = data;
    for (size_t index = 0U; index < count; ++index) {
        hash ^= bytes[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}
static uint64_t request_hash_string(uint64_t hash,const char *text)
{
    return request_hash_bytes(hash,text,strlen(text) + 1U);
}
/* Explicit little-endian scalar encoding avoids enum width, object padding and
 * host byte-order dependence. Text remains literal; path aliases are not resolved. */
static uint64_t request_hash_scalar(uint64_t hash,uint64_t value)
{
    unsigned char bytes[8U];
    for (size_t index = 0U; index < sizeof(bytes); ++index)
        bytes[index] = (unsigned char)(value >> (index * 8U));
    return request_hash_bytes(hash,bytes,sizeof(bytes));
}
uint64_t umi_compiler_request_hash(const UmiCompilerRequest *request)
{
    if (!umi_compiler_request_storage_valid(request)) return 0U;
    uint64_t hash = UINT64_C(1469598103934665603);
    hash = request_hash_string(hash,"umicom.compiler.request/2");
    hash = request_hash_string(hash,request->project_id);
    hash = request_hash_string(hash,request->unit_id);
    hash = request_hash_string(hash,request->source_path);
    hash = request_hash_string(hash,request->output_path);
    hash = request_hash_string(hash,request->working_directory);
    hash = request_hash_string(hash,request->standard);
    hash = request_hash_scalar(hash,(uint32_t)request->language);
    hash = request_hash_scalar(hash,(uint32_t)request->action);
    hash = request_hash_string(hash,request->target.triple);
    hash = request_hash_string(hash,request->target.architecture);
    hash = request_hash_string(hash,request->target.vendor);
    hash = request_hash_string(hash,request->target.operating_system);
    hash = request_hash_string(hash,request->target.environment);
    hash = request_hash_scalar(hash,(uint32_t)request->target.object_format);
    hash = request_hash_scalar(hash,request->target.pointer_width);
    hash = request_hash_scalar(hash,request->target.little_endian ? 1U : 0U);
    hash = request_hash_scalar(hash,request->debug_information ? 1U : 0U);
    hash = request_hash_scalar(hash,request->position_independent ? 1U : 0U);
    hash = request_hash_scalar(hash,request->warnings_as_errors ? 1U : 0U);
    /* Group names and counts prevent a value migrating between collections from
     * retaining the same serialized identity. Order within each group matters. */
    hash = request_hash_string(hash,"includes");
    hash = request_hash_scalar(hash,request->include_directory_count);
    for (size_t index = 0U; index < request->include_directory_count; ++index)
        hash = request_hash_string(hash,request->include_directories[index]);
    hash = request_hash_string(hash,"definitions");
    hash = request_hash_scalar(hash,request->definition_count);
    for (size_t index = 0U; index < request->definition_count; ++index)
        hash = request_hash_string(hash,request->definitions[index]);
    hash = request_hash_string(hash,"options");
    hash = request_hash_scalar(hash,request->option_count);
    for (size_t index = 0U; index < request->option_count; ++index)
        hash = request_hash_string(hash,request->options[index]);
    /* Zero is reserved for invalid storage, including in the cache entry API.
     * Mapping this one result does not remove the general possibility of collisions. */
    return hash == 0U ? UINT64_MAX : hash;
}
bool umi_compiler_cache_entry_valid(const UmiCompilerCacheEntry *entry)
{
    return entry != NULL && entry->key != 0U &&
        UmiCompilerCatalogueText(entry->artifact_path,sizeof(entry->artifact_path),true);
}
bool umi_compiler_cache_valid(const UmiCompilerCache *cache)
{
    if (cache == NULL || cache->count > UMI_COMPILER_MAX_CACHE_ENTRIES) return false;
    for (size_t index = 0U; index < cache->count; ++index)
        if (!umi_compiler_cache_entry_valid(&cache->items[index])) return false;
    return true;
}
UmiStatus umi_compiler_cache_put(UmiCompilerCache *cache,const UmiCompilerCacheEntry *entry)
{
    if (cache == NULL || !umi_compiler_cache_entry_valid(entry)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_cache_valid(cache)) return UMI_STATUS_INVALID_STATE;
    if (cache->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < cache->count; ++index) {
        if (cache->items[index].key != entry->key) continue;
        (void)memmove(&cache->items[index],entry,sizeof(*entry));
        cache->revision += 1U;
        return UMI_STATUS_OK;
    }
    if (cache->count == UMI_COMPILER_MAX_CACHE_ENTRIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memmove(&cache->items[cache->count],entry,sizeof(*entry));
    cache->count += 1U;
    cache->revision += 1U;
    return UMI_STATUS_OK;
}
const UmiCompilerCacheEntry *umi_compiler_cache_get(UmiCompilerCache *cache,uint64_t key)
{
    if (key == 0U || !umi_compiler_cache_valid(cache)) return NULL;
    for (size_t index = 0U; index < cache->count; ++index) {
        if (cache->items[index].key != key || !cache->items[index].valid) continue;
        /* Telemetry saturates independently of mutation revision. An exhausted
         * statistic must neither wrap nor prevent a valid read. */
        if (cache->hits != UINT64_MAX) cache->hits += 1U;
        return &cache->items[index];
    }
    if (cache->misses != UINT64_MAX) cache->misses += 1U;
    return NULL;
}
UmiStatus umi_compiler_cache_invalidate_checked(UmiCompilerCache *cache,uint64_t older_than_ns)
{
    if (cache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_cache_valid(cache)) return UMI_STATUS_INVALID_STATE;
    if (cache->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < cache->count; ++index)
        if (cache->items[index].created_at_ns < older_than_ns) cache->items[index].valid = false;
    /* Retain the existing operation revision, even if no row met the cutoff. */
    cache->revision += 1U;
    return UMI_STATUS_OK;
}
void umi_compiler_cache_invalidate(UmiCompilerCache *cache,uint64_t older_than_ns)
{
    /* Compatibility callers retain the void interface. New clients should use
     * the checked form to distinguish refusal from completed invalidation. */
    (void)umi_compiler_cache_invalidate_checked(cache,older_than_ns);
}
