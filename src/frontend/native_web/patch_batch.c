/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/patch_batch.c
 *
 * PURPOSE:
 *   Own an ordered bounded patch collection for one browser render revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/patch_batch.h"

#include <string.h>
void umi_native_web_patch_batch_init(UmiNativeWebPatchBatch *batch,uint64_t revision){if(batch!=NULL){(void)memset(batch,0,sizeof(*batch));batch->revision=revision;}}
UmiStatus umi_native_web_patch_batch_add(UmiNativeWebPatchBatch *batch,const UmiNativeWebPatch *patch){if(batch==NULL||patch==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(batch->count>=UMI_NATIVE_WEB_MAX_PATCHES)return UMI_STATUS_CAPACITY_EXCEEDED;batch->items[batch->count++]=*patch;return UMI_STATUS_OK;}

