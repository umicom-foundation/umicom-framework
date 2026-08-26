/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_cache.h
 *
 * PURPOSE:
 *   Cache visible row revisions by stable row key.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_CACHE_H
#define UMICOM_UI_ENTERPRISE_ROW_CACHE_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntRowCacheEntry { uint64_t row_key; uint64_t revision; uint64_t stamp; } UmiUiEntRowCacheEntry;
typedef struct UmiUiEntRowCache { UmiUiEntRowCacheEntry entries[UMI_UI_ENT_MAX_CACHE]; size_t count; uint64_t clock; } UmiUiEntRowCache;
void umi_ui_ent_row_cache_init(UmiUiEntRowCache *c);
UmiStatus umi_ui_ent_row_cache_put(UmiUiEntRowCache *c,uint64_t key,uint64_t revision);
int umi_ui_ent_row_cache_get(const UmiUiEntRowCache *c,uint64_t key,uint64_t *out_revision);

#endif
