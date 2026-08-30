/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_budget.c
 *
 * PURPOSE:
 *   Apply patch-count and payload-size budgets to protect interactive sessions from runaway renders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch_budget.h"

#include <string.h>
UmiStatus umi_native_web_patch_budget_check(const UmiNativeWebPatchBatch *batch,const UmiNativeWebPatchBudget *budget,size_t *out_estimated_bytes){size_t i,total=2U;if(batch==NULL||budget==NULL||out_estimated_bytes==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<batch->count;++i)total+=32U+strlen(batch->items[i].target_id)+strlen(batch->items[i].name)+strlen(batch->items[i].value);*out_estimated_bytes=total;if(batch->count>budget->max_patches||total>budget->max_estimated_bytes)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}

