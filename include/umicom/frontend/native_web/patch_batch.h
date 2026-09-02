/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/patch_batch.h
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

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_BATCH_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_BATCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/patch.h"
/**
 * Represent the native web patch batch data shared with callers of this public contract.
 */
typedef struct UmiNativeWebPatchBatch { UmiNativeWebPatch items[UMI_NATIVE_WEB_MAX_PATCHES]; size_t count; uint64_t revision; } UmiNativeWebPatchBatch;
/* Initialise an empty patch batch for a render revision. */
void umi_native_web_patch_batch_init(UmiNativeWebPatchBatch *batch, uint64_t revision);
/* Append one patch in deterministic order. */
UmiStatus umi_native_web_patch_batch_add(UmiNativeWebPatchBatch *batch, const UmiNativeWebPatch *patch);

#ifdef __cplusplus
}
#endif
#endif
