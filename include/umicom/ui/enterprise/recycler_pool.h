/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/recycler_pool.h
 *
 * PURPOSE:
 *   Track reusable renderer slots without coupling them to toolkit widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_RECYCLER_POOL_H
#define UMICOM_UI_ENTERPRISE_RECYCLER_POOL_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent recycler pool data shared with callers of this public contract.
 */
typedef struct UmiUiEntRecyclerPool { size_t capacity; size_t in_use; uint64_t generation; } UmiUiEntRecyclerPool;
/**
 * Initialise ui ent recycler pool from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_recycler_pool_init(UmiUiEntRecyclerPool *pool,size_t capacity);
/**
 * Provide the ui ent recycler pool acquire operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_recycler_pool_acquire(UmiUiEntRecyclerPool *pool,size_t *out_slot);
/**
 * Release or reset state held by ui ent recycler pool so the same storage can be reused
 * safely.
 */
UmiStatus umi_ui_ent_recycler_pool_release(UmiUiEntRecyclerPool *pool);

#endif
