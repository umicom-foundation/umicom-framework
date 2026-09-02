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
/**
 * Represent the ui ent cell cache entry data shared with callers of this public contract.
 */
typedef struct UmiUiEntCellCacheEntry { uint64_t row_key; size_t column; UmiUiEntCellValue value; uint64_t stamp; } UmiUiEntCellCacheEntry;
/**
 * Represent the ui ent cell cache data shared with callers of this public contract.
 */
typedef struct UmiUiEntCellCache { UmiUiEntCellCacheEntry entries[UMI_UI_ENT_MAX_CACHE]; size_t count; uint64_t clock; } UmiUiEntCellCache;
/**
 * Initialise ui ent cell cache from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_cell_cache_init(UmiUiEntCellCache *c);
/**
 * Provide the ui ent cell cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_cell_cache_put(UmiUiEntCellCache *c,uint64_t key,size_t column,const UmiUiEntCellValue *value);
/**
 * Provide the ui ent cell cache get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_cell_cache_get(const UmiUiEntCellCache *c,uint64_t key,size_t column,UmiUiEntCellValue *out_value);

#endif
