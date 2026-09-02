/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/cache.h
 *
 * PURPOSE:
 *   Publish the public cache contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_CACHE_H
#define UMICOM_COMPILER_CACHE_H
#include "umicom/compiler/request.h"
#define UMI_COMPILER_MAX_CACHE_ENTRIES 512U
/**
 * Represent the compiler cache entry data shared with callers of this public contract.
 */
typedef struct UmiCompilerCacheEntry { uint64_t key; char artifact_path[UMI_COMPILER_PATH_CAPACITY]; uint64_t source_timestamp_ns; uint64_t created_at_ns; bool valid; } UmiCompilerCacheEntry;
/**
 * Represent the compiler cache data shared with callers of this public contract.
 */
typedef struct UmiCompilerCache { UmiCompilerCacheEntry items[UMI_COMPILER_MAX_CACHE_ENTRIES]; size_t count; uint64_t hits; uint64_t misses; uint64_t revision; } UmiCompilerCache;
/**
 * Provide the compiler request hash operation used by this module and its client
 * applications.
 */
uint64_t umi_compiler_request_hash(const UmiCompilerRequest *request);
/**
 * Provide the compiler cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_cache_put(UmiCompilerCache *cache,const UmiCompilerCacheEntry *entry);
/**
 * Provide the compiler cache get operation used by this module and its client
 * applications.
 */
const UmiCompilerCacheEntry *umi_compiler_cache_get(UmiCompilerCache *cache,uint64_t key);
/**
 * Provide the compiler cache invalidate operation used by this module and its client
 * applications.
 */
void umi_compiler_cache_invalidate(UmiCompilerCache *cache,uint64_t older_than_ns);
#endif
