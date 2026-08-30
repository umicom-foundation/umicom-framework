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
UmiStatus umi_native_web_patch_coalesce(UmiNativeWebPatchBatch *batch){size_t i,j,k;if(batch==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<batch->count;++i){for(j=i+1U;j<batch->count;){if(umi_native_web_patch_same_slot(&batch->items[i],&batch->items[j])){batch->items[i]=batch->items[j];for(k=j+1U;k<batch->count;++k)batch->items[k-1U]=batch->items[k];--batch->count;}else ++j;}}return UMI_STATUS_OK;}

