/* Umicom Framework | Compiler cache metadata | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_CACHE_H
#define UMICOM_COMPILER_CACHE_H
#include "umicom/compiler/request.h"
#define UMI_COMPILER_MAX_CACHE_ENTRIES 512U
typedef struct UmiCompilerCacheEntry { uint64_t key; char artifact_path[UMI_COMPILER_PATH_CAPACITY]; uint64_t source_timestamp_ns; uint64_t created_at_ns; bool valid; } UmiCompilerCacheEntry;
typedef struct UmiCompilerCache { UmiCompilerCacheEntry items[UMI_COMPILER_MAX_CACHE_ENTRIES]; size_t count; uint64_t hits; uint64_t misses; uint64_t revision; } UmiCompilerCache;
uint64_t umi_compiler_request_hash(const UmiCompilerRequest *request);
UmiStatus umi_compiler_cache_put(UmiCompilerCache *cache,const UmiCompilerCacheEntry *entry);
const UmiCompilerCacheEntry *umi_compiler_cache_get(UmiCompilerCache *cache,uint64_t key);
void umi_compiler_cache_invalidate(UmiCompilerCache *cache,uint64_t older_than_ns);
#endif
