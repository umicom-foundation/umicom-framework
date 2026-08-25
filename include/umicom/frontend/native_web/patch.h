/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/patch.h
 *
 * PURPOSE:
 *   Define one semantic DOM mutation with target, attribute/value and revision metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebPatch { UmiNativeWebPatchKind kind; char target_id[UMI_NATIVE_WEB_ID_CAPACITY]; char name[64]; char value[UMI_NATIVE_WEB_VALUE_CAPACITY]; int32_t order; uint64_t revision; } UmiNativeWebPatch;
/* Initialise one validated DOM patch. */
UmiStatus umi_native_web_patch_init(UmiNativeWebPatch *patch, UmiNativeWebPatchKind kind, const char *target_id, const char *name, const char *value, uint64_t revision);
/* Return true when two patches address the same semantic mutation slot. */
bool umi_native_web_patch_same_slot(const UmiNativeWebPatch *left, const UmiNativeWebPatch *right);

#ifdef __cplusplus
}
#endif
#endif
