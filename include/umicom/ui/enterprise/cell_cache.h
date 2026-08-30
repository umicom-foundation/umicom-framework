/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/cell_cache.h
 *
 * PURPOSE:
 *   Cache typed cells by stable row key and column index.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CELL_CACHE_H
#define UMICOM_UI_ENTERPRISE_CELL_CACHE_H

#include "umicom/ui/enterprise/cell_value.h"
typedef struct UmiUiEntCellCacheEntry { uint64_t row_key; size_t column; UmiUiEntCellValue value; uint64_t stamp; } UmiUiEntCellCacheEntry;
typedef struct UmiUiEntCellCache { UmiUiEntCellCacheEntry entries[UMI_UI_ENT_MAX_CACHE]; size_t count; uint64_t clock; } UmiUiEntCellCache;
void umi_ui_ent_cell_cache_init(UmiUiEntCellCache *c);
UmiStatus umi_ui_ent_cell_cache_put(UmiUiEntCellCache *c,uint64_t key,size_t column,const UmiUiEntCellValue *value);
int umi_ui_ent_cell_cache_get(const UmiUiEntCellCache *c,uint64_t key,size_t column,UmiUiEntCellValue *out_value);

#endif
