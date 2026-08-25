/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_cache.c
 *
 * PURPOSE:
 *   Cache bounded query-plan fingerprints and hit evidence without caching application-owned result objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_cache.h"
#include <string.h>

/* Cache metadata remains separate from backend statement/result ownership. */ void umi_data_query_cache_init(UmiDataQueryCache *cache){if(cache!=NULL)(void)memset(cache,0,sizeof(*cache));}
/* Upsert makes invalidation/reprepare explicit after schema changes. */ UmiStatus umi_data_query_cache_put(UmiDataQueryCache *cache,const char *cache_id,uint64_t fingerprint,uint64_t created_at){size_t i;UmiStatus s;if(cache==NULL||cache_id==NULL||fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<cache->count;++i)if(strcmp(cache->items[i].cache_id,cache_id)==0){cache->items[i].plan_fingerprint=fingerprint;cache->items[i].created_at=created_at;cache->items[i].valid=true;return UMI_STATUS_OK;}if(cache->count>=UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;(void)memset(&cache->items[cache->count],0,sizeof(cache->items[cache->count]));s=umi_data_enterprise_copy_text(cache->items[cache->count].cache_id,sizeof(cache->items[cache->count].cache_id),cache_id);if(s!=UMI_STATUS_OK)return s;cache->items[cache->count].plan_fingerprint=fingerprint;cache->items[cache->count].created_at=created_at;cache->items[cache->count].valid=true;cache->count++;return UMI_STATUS_OK;}
/* Hit accounting is useful to observability without leaking cached backend handles. */ UmiStatus umi_data_query_cache_hit(UmiDataQueryCache *cache,const char *cache_id,uint64_t *out_fingerprint){size_t i;if(cache==NULL||cache_id==NULL||out_fingerprint==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<cache->count;++i)if(strcmp(cache->items[i].cache_id,cache_id)==0&&cache->items[i].valid){cache->items[i].hits++;*out_fingerprint=cache->items[i].plan_fingerprint;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/* Schema/migration changes can invalidate all prepared-plan evidence in one deterministic operation. */ void umi_data_query_cache_invalidate_all(UmiDataQueryCache *cache){size_t i;if(cache==NULL)return;for(i=0;i<cache->count;++i)cache->items[i].valid=false;}
