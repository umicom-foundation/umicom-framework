/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/recycler_pool.c
 *
 * PURPOSE:
 *   Implement bounded reusable slot accounting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/recycler_pool.h"
/*
 * Initialise ui ent recycler pool from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_recycler_pool_init(UmiUiEntRecyclerPool *p,size_t c){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||c==0U)return UMI_STATUS_INVALID_ARGUMENT;p->capacity=c;p->in_use=0U;p->generation=1U;return UMI_STATUS_OK;}
/*
 * Provide the ui ent recycler pool acquire operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_recycler_pool_acquire(UmiUiEntRecyclerPool *p,size_t *slot){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!slot)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->in_use>=p->capacity)return UMI_STATUS_CAPACITY_EXCEEDED;*slot=p->in_use++;p->generation++;return UMI_STATUS_OK;}
/*
 * Release or reset state held by ui ent recycler pool so the same storage can be reused
 * safely.
 */
UmiStatus umi_ui_ent_recycler_pool_release(UmiUiEntRecyclerPool *p){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!p)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(p->in_use==0U)return UMI_STATUS_INVALID_STATE;p->in_use--;p->generation++;return UMI_STATUS_OK;}
