/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_cache.c
 *
 * PURPOSE:
 *   Cache valuation outputs using deterministic product/market/model fingerprints.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_cache.h"

#include <string.h>
void umi_enterprise_valuation_cache_init(UmiEnterpriseValuationCache *c){ if(c!=NULL)memset(c,0,sizeof *c); }
UmiStatus umi_enterprise_valuation_cache_put(UmiEnterpriseValuationCache *c,const UmiEnterpriseValuationCacheEntry *e){ size_t i; if(c==NULL||e==NULL||!umi_quant_number_valid(e->present_value)||e->key.fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<c->count;++i)if(c->entries[i].key.fingerprint==e->key.fingerprint){c->entries[i]=*e;return UMI_STATUS_OK;} if(c->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; c->entries[c->count++]=*e;return UMI_STATUS_OK; }
const UmiEnterpriseValuationCacheEntry *umi_enterprise_valuation_cache_find(const UmiEnterpriseValuationCache *c,uint64_t fp){ size_t i; if(c==NULL||fp==0U)return NULL; for(i=0U;i<c->count;++i)if(c->entries[i].key.fingerprint==fp)return &c->entries[i]; return NULL; }
