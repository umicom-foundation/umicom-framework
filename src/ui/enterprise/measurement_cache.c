/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/measurement_cache.c
 *
 * PURPOSE:
 *   Implement bounded adaptive measurement lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/measurement_cache.h"
#include <string.h>
void umi_ui_ent_measurement_cache_init(UmiUiEntMeasurementCache *c){if(c)memset(c,0,sizeof *c);}
UmiStatus umi_ui_ent_measurement_cache_set(UmiUiEntMeasurementCache *c,const char *id,int32_t px,uint64_t rev){size_t i;if(!c||!umi_ui_ent_id_valid(id)||px<=0)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<c->count;i++)if(strcmp(c->items[i].id,id)==0){c->items[i].pixels=px;c->items[i].revision=rev;return UMI_STATUS_OK;}if(c->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(c->items[c->count].id,sizeof c->items[c->count].id,id);c->items[c->count].pixels=px;c->items[c->count].revision=rev;c->count++;return UMI_STATUS_OK;}
int umi_ui_ent_measurement_cache_get(const UmiUiEntMeasurementCache *c,const char *id,int32_t *out){size_t i;if(!c||!id||!out)return 0;for(i=0;i<c->count;i++)if(strcmp(c->items[i].id,id)==0){*out=c->items[i].pixels;return 1;}return 0;}
