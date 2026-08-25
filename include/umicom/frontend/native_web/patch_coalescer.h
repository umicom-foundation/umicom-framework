/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/patch_coalescer.h
 *
 * PURPOSE:
 *   Coalesce superseded text/attribute mutations before sending a browser update.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_COALESCER_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_COALESCER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/patch_batch.h"
/* Remove superseded mutations while preserving the newest patch for each semantic slot. */
UmiStatus umi_native_web_patch_coalesce(UmiNativeWebPatchBatch *batch);

#ifdef __cplusplus
}
#endif
#endif
