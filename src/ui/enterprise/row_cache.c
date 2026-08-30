/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_cache.c
 *
 * PURPOSE:
 *   Implement bounded least-recently-written row revision caching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_cache.h"
#include <string.h>
void umi_ui_ent_row_cache_init(UmiUiEntRowCache *c){if(c)memset(c,0,sizeof *c);}
UmiStatus umi_ui_ent_row_cache_put(UmiUiEntRowCache *c,uint64_t k,uint64_t r){size_t i,slot=0;if(!c||k==0U)return UMI_STATUS_INVALID_ARGUMENT;c->clock++;for(i=0;i<c->count;i++)if(c->entries[i].row_key==k){c->entries[i].revision=r;c->entries[i].stamp=c->clock;return UMI_STATUS_OK;}if(c->count<UMI_UI_ENT_MAX_CACHE)slot=c->count++;else{for(i=1;i<c->count;i++)if(c->entries[i].stamp<c->entries[slot].stamp)slot=i;}c->entries[slot]=(UmiUiEntRowCacheEntry){k,r,c->clock};return UMI_STATUS_OK;}
int umi_ui_ent_row_cache_get(const UmiUiEntRowCache *c,uint64_t k,uint64_t *out){size_t i;if(!c||!out)return 0;for(i=0;i<c->count;i++)if(c->entries[i].row_key==k){*out=c->entries[i].revision;return 1;}return 0;}
