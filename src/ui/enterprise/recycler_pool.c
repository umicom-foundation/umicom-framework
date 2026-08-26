/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/recycler_pool.c
 *
 * PURPOSE:
 *   Implement bounded reusable slot accounting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/recycler_pool.h"
UmiStatus umi_ui_ent_recycler_pool_init(UmiUiEntRecyclerPool *p,size_t c){if(!p||c==0U)return UMI_STATUS_INVALID_ARGUMENT;p->capacity=c;p->in_use=0U;p->generation=1U;return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_recycler_pool_acquire(UmiUiEntRecyclerPool *p,size_t *slot){if(!p||!slot)return UMI_STATUS_INVALID_ARGUMENT;if(p->in_use>=p->capacity)return UMI_STATUS_CAPACITY_EXCEEDED;*slot=p->in_use++;p->generation++;return UMI_STATUS_OK;}
UmiStatus umi_ui_ent_recycler_pool_release(UmiUiEntRecyclerPool *p){if(!p)return UMI_STATUS_INVALID_ARGUMENT;if(p->in_use==0U)return UMI_STATUS_INVALID_STATE;p->in_use--;p->generation++;return UMI_STATUS_OK;}
