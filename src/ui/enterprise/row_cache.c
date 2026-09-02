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
/*
 * Initialise ui ent row cache from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_row_cache_init(UmiUiEntRowCache *c){/* Apply this branch only when its contract condition is satisfied. */ if(c)memset(c,0,sizeof *c);}
/*
 * Provide the ui ent row cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_row_cache_put(UmiUiEntRowCache *c,uint64_t k,uint64_t r){size_t i,slot=0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||k==0U)return UMI_STATUS_INVALID_ARGUMENT;c->clock++;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->entries[i].row_key==k){c->entries[i].revision=r;c->entries[i].stamp=c->clock;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->count<UMI_UI_ENT_MAX_CACHE)slot=c->count++;/* Use this fallback path when the earlier condition does not apply. */ else{/* Visit each bounded item once so every record receives the same rule. */ for(i=1;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->entries[i].stamp<c->entries[slot].stamp)slot=i;}c->entries[slot]=(UmiUiEntRowCacheEntry){k,r,c->clock};return UMI_STATUS_OK;}
/*
 * Provide the ui ent row cache get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_row_cache_get(const UmiUiEntRowCache *c,uint64_t k,uint64_t *out){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!out)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->entries[i].row_key==k){*out=c->entries[i].revision;return 1;}return 0;}
