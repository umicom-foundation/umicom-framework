/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_cache.h
 *
 * PURPOSE:
 *   Cache context evidence by stable fingerprint and revision.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_CACHE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_CACHE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context cache entry data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevContextCacheEntry { uint64_t key; uint64_t revision; uint32_t score; int valid; } UmiAiDevContextCacheEntry;
/**
 * Represent the ai dev context cache data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextCache { UmiAiDevContextCacheEntry entries[UMI_AI_DEV_SMALL_CAPACITY]; size_t next_slot; size_t count; } UmiAiDevContextCache;
/**
 * Initialise ai dev context cache from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_context_cache_init(UmiAiDevContextCache *cache);
/**
 * Provide the ai dev context cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_cache_put(UmiAiDevContextCache *cache,uint64_t key,uint64_t revision,uint32_t score);
/**
 * Provide the ai dev context cache get operation used by this module and its client
 * applications.
 */
int umi_ai_dev_context_cache_get(const UmiAiDevContextCache *cache,uint64_t key,uint64_t revision,uint32_t *out_score);
/**
 * Provide the ai dev context cache invalidate before operation used by this module and its
 * client applications.
 */
void umi_ai_dev_context_cache_invalidate_before(UmiAiDevContextCache *cache,uint64_t minimum_revision);

#ifdef __cplusplus
}
#endif

#endif
