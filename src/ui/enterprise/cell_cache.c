/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_cache.c
 *
 * PURPOSE:
 *   Implement bounded cell caching for virtual renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_cache.h"
#include <string.h>
/*
 * Initialise ui ent cell cache from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_cell_cache_init(UmiUiEntCellCache *c){/* Apply this branch only when its contract condition is satisfied. */ if(c)memset(c,0,sizeof *c);}
/*
 * Provide the ui ent cell cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_cell_cache_put(UmiUiEntCellCache *c,uint64_t k,size_t col,const UmiUiEntCellValue *v){size_t i,slot=0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!v||k==0U)return UMI_STATUS_INVALID_ARGUMENT;c->clock++;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->entries[i].row_key==k&&c->entries[i].column==col){c->entries[i].value=*v;c->entries[i].stamp=c->clock;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->count<UMI_UI_ENT_MAX_CACHE)slot=c->count++;/* Use this fallback path when the earlier condition does not apply. */ else{/* Visit each bounded item once so every record receives the same rule. */ for(i=1;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->entries[i].stamp<c->entries[slot].stamp)slot=i;}c->entries[slot]=(UmiUiEntCellCacheEntry){k,col,*v,c->clock};return UMI_STATUS_OK;}
/*
 * Provide the ui ent cell cache get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_cell_cache_get(const UmiUiEntCellCache *c,uint64_t k,size_t col,UmiUiEntCellValue *out){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!out)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->entries[i].row_key==k&&c->entries[i].column==col){*out=c->entries[i].value;return 1;}return 0;}
