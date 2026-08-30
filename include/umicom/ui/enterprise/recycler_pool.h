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
typedef struct UmiUiEntRecyclerPool { size_t capacity; size_t in_use; uint64_t generation; } UmiUiEntRecyclerPool;
UmiStatus umi_ui_ent_recycler_pool_init(UmiUiEntRecyclerPool *pool,size_t capacity);
UmiStatus umi_ui_ent_recycler_pool_acquire(UmiUiEntRecyclerPool *pool,size_t *out_slot);
UmiStatus umi_ui_ent_recycler_pool_release(UmiUiEntRecyclerPool *pool);

#endif
