/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_cache.h
 *
 * PURPOSE:
 *   Cache visible row revisions by stable row key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_CACHE_H
#define UMICOM_UI_ENTERPRISE_ROW_CACHE_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent row cache entry data shared with callers of this public contract.
 */
typedef struct UmiUiEntRowCacheEntry { uint64_t row_key; uint64_t revision; uint64_t stamp; } UmiUiEntRowCacheEntry;
/**
 * Represent the ui ent row cache data shared with callers of this public contract.
 */
typedef struct UmiUiEntRowCache { UmiUiEntRowCacheEntry entries[UMI_UI_ENT_MAX_CACHE]; size_t count; uint64_t clock; } UmiUiEntRowCache;
/**
 * Initialise ui ent row cache from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_row_cache_init(UmiUiEntRowCache *c);
/**
 * Provide the ui ent row cache put operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_row_cache_put(UmiUiEntRowCache *c,uint64_t key,uint64_t revision);
/**
 * Provide the ui ent row cache get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_row_cache_get(const UmiUiEntRowCache *c,uint64_t key,uint64_t *out_revision);

#endif
