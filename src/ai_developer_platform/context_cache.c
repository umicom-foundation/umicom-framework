/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/context_cache.c
 *
 * PURPOSE:
 *   Cache context evidence by stable fingerprint and revision.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_cache.h"
#include <string.h>
void umi_ai_dev_context_cache_init(UmiAiDevContextCache *cache) {
    if (cache != NULL) {
        memset(cache, 0, sizeof(*cache));
    }
}
UmiStatus umi_ai_dev_context_cache_put(UmiAiDevContextCache *cache,uint64_t key,uint64_t revision,uint32_t score) { size_t i; if(cache==NULL||key==0U) return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<UMI_AI_DEV_SMALL_CAPACITY;++i) if(cache->entries[i].valid&&cache->entries[i].key==key) { cache->entries[i].revision=revision; cache->entries[i].score=score; return UMI_STATUS_OK; } i=cache->next_slot; cache->entries[i].key=key; cache->entries[i].revision=revision; cache->entries[i].score=score; cache->entries[i].valid=1; cache->next_slot=(i+1U)%UMI_AI_DEV_SMALL_CAPACITY; if(cache->count<UMI_AI_DEV_SMALL_CAPACITY) ++cache->count; return UMI_STATUS_OK; }
int umi_ai_dev_context_cache_get(const UmiAiDevContextCache *cache,uint64_t key,uint64_t revision,uint32_t *out_score) { size_t i; if(cache==NULL||out_score==NULL) return 0; for(i=0U;i<UMI_AI_DEV_SMALL_CAPACITY;++i) if(cache->entries[i].valid&&cache->entries[i].key==key&&cache->entries[i].revision==revision) { *out_score=cache->entries[i].score; return 1; } return 0; }
void umi_ai_dev_context_cache_invalidate_before(UmiAiDevContextCache *cache,uint64_t minimum_revision) { size_t i; if(cache==NULL) return; for(i=0U;i<UMI_AI_DEV_SMALL_CAPACITY;++i) if(cache->entries[i].valid&&cache->entries[i].revision<minimum_revision) { cache->entries[i].valid=0; if(cache->count>0U)--cache->count; } }
