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

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
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

/** Admit every active key/path, including invalidated rows. The valid flag is a
 * caller assertion, not a filesystem check. Stored paths must remain terminated. */
bool umi_compiler_cache_entry_valid(const UmiCompilerCacheEntry *entry);
bool umi_compiler_cache_valid(const UmiCompilerCache *cache);
/** Checked form of the existing strict created_at_ns cutoff. The operation
 * increments revision even when no rows match. Invalid owners and maximum
 * revision are refused without changing any bytes. The void compatibility API
 * delegates here and cannot report refusal. Callers must serialize access. */
UmiStatus umi_compiler_cache_invalidate_checked(UmiCompilerCache *cache,uint64_t older_than_ns);
/** Request hashes now include every public metadata field and collection boundary,
 * with fixed-width scalar encoding. Old persisted keys must be rebuilt. Invalid
 * storage returns zero; bounded drafts can be hashed without being compilable.
 * This non-cryptographic key may collide and excludes source/dependency contents,
 * compiler identity and environment. A hit alone never proves an artifact reusable.
 *
 * Put replaces an existing key even in a full cache, but refuses any write when
 * revision is exhausted. Get admits the whole owner before recording telemetry;
 * hits/misses saturate at UINT64_MAX. Returned rows are borrowed until mutation. */
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
