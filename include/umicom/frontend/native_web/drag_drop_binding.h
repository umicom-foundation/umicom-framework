/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/drag_drop_binding.h
 *
 * PURPOSE:
 *   Normalize browser drag/drop operations into semantic layout-edit intents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_DRAG_DROP_BINDING_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_DRAG_DROP_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
/**
 * Represent the native web drag drop binding data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeWebDragDropBinding { char source_id[UMI_NATIVE_WEB_ID_CAPACITY]; char target_id[UMI_NATIVE_WEB_ID_CAPACITY]; char payload_type[64]; bool allowed; } UmiNativeWebDragDropBinding;
/* Initialise validated drag drop binding state. */
UmiStatus umi_native_web_drag_drop_binding_init(UmiNativeWebDragDropBinding *binding, const char *source_id, const char *target_id, const char *payload_type, bool allowed);

#ifdef __cplusplus
}
#endif
#endif
