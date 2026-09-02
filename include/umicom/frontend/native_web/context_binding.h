/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/context_binding.h
 *
 * PURPOSE:
 *   Bind browser selections to typed Framework context-channel identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_CONTEXT_BINDING_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_CONTEXT_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
/**
 * Represent the native web context binding data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeWebContextBinding { char element_id[UMI_NATIVE_WEB_ID_CAPACITY]; char channel_id[UMI_NATIVE_WEB_ID_CAPACITY]; char context_type[64]; bool bidirectional; } UmiNativeWebContextBinding;
/* Initialise validated context binding state. */
UmiStatus umi_native_web_context_binding_init(UmiNativeWebContextBinding *binding, const char *element_id, const char *channel_id, const char *context_type, bool bidirectional);

#ifdef __cplusplus
}
#endif
#endif
