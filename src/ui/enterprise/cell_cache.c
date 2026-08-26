/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_cache.c
 *
 * PURPOSE:
 *   Implement bounded cell caching for virtual renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_cache.h"
#include <string.h>
void umi_ui_ent_cell_cache_init(UmiUiEntCellCache *c){if(c)memset(c,0,sizeof *c);}
UmiStatus umi_ui_ent_cell_cache_put(UmiUiEntCellCache *c,uint64_t k,size_t col,const UmiUiEntCellValue *v){size_t i,slot=0;if(!c||!v||k==0U)return UMI_STATUS_INVALID_ARGUMENT;c->clock++;for(i=0;i<c->count;i++)if(c->entries[i].row_key==k&&c->entries[i].column==col){c->entries[i].value=*v;c->entries[i].stamp=c->clock;return UMI_STATUS_OK;}if(c->count<UMI_UI_ENT_MAX_CACHE)slot=c->count++;else{for(i=1;i<c->count;i++)if(c->entries[i].stamp<c->entries[slot].stamp)slot=i;}c->entries[slot]=(UmiUiEntCellCacheEntry){k,col,*v,c->clock};return UMI_STATUS_OK;}
int umi_ui_ent_cell_cache_get(const UmiUiEntCellCache *c,uint64_t k,size_t col,UmiUiEntCellValue *out){size_t i;if(!c||!out)return 0;for(i=0;i<c->count;i++)if(c->entries[i].row_key==k&&c->entries[i].column==col){*out=c->entries[i].value;return 1;}return 0;}
