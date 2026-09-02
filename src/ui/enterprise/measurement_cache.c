/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/measurement_cache.c
 *
 * PURPOSE:
 *   Implement bounded adaptive measurement lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/measurement_cache.h"
#include <string.h>
/*
 * Initialise ui ent measurement cache from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_measurement_cache_init(UmiUiEntMeasurementCache *c){/* Apply this branch only when its contract condition is satisfied. */ if(c)memset(c,0,sizeof *c);}
/*
 * Copy ui ent measurement cache into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_ent_measurement_cache_set(UmiUiEntMeasurementCache *c,const char *id,int32_t px,uint64_t rev){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!umi_ui_ent_id_valid(id)||px<=0)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(c->items[i].id,id)==0){c->items[i].pixels=px;c->items[i].revision=rev;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(c->items[c->count].id,sizeof c->items[c->count].id,id);c->items[c->count].pixels=px;c->items[c->count].revision=rev;c->count++;return UMI_STATUS_OK;}
/*
 * Provide the ui ent measurement cache get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_measurement_cache_get(const UmiUiEntMeasurementCache *c,const char *id,int32_t *out){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!c||!id||!out)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<c->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(c->items[i].id,id)==0){*out=c->items[i].pixels;return 1;}return 0;}
