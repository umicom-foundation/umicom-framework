/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_coalescer.c
 *
 * PURPOSE:
 *   Coalesce superseded text/attribute mutations before sending a browser update.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch_coalescer.h"

#include <string.h>
/*
 * Provide the native web patch coalesce operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_patch_coalesce(UmiNativeWebPatchBatch *batch){size_t i,j,k;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<batch->count;++i){/* Visit each bounded item once so every record receives the same rule. */ for(j=i+1U;j<batch->count;){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_native_web_patch_same_slot(&batch->items[i],&batch->items[j])){batch->items[i]=batch->items[j];/* Visit each bounded item once so every record receives the same rule. */ for(k=j+1U;k<batch->count;++k)batch->items[k-1U]=batch->items[k];--batch->count;}/* Use this fallback path when the earlier condition does not apply. */ else ++j;}}return UMI_STATUS_OK;}

