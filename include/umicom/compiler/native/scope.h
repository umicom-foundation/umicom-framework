/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/scope.h
 *
 * PURPOSE:
 *   Represent lexical scope ancestry and bounded symbol membership for C name lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SCOPE_H
#define UMICOM_COMPILER_NATIVE_SCOPE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_SCOPE_SYMBOLS 64U
/**
 * Represent the native scope data shared with callers of this public contract.
 */
typedef struct UmiNativeScope { uint32_t id; uint32_t parent_id; uint32_t depth; uint32_t symbol_ids[UMI_NC_MAX_SCOPE_SYMBOLS]; size_t symbol_count; } UmiNativeScope;
/**
 * Initialise nc scope from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_scope_init(UmiNativeScope *scope,uint32_t id,uint32_t parent_id,uint32_t depth);
/**
 * Provide the nc scope add symbol operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_scope_add_symbol(UmiNativeScope *scope,uint32_t symbol_id);
/**
 * Provide the nc scope contains symbol operation used by this module and its client
 * applications.
 */
bool umi_nc_scope_contains_symbol(const UmiNativeScope *scope,uint32_t symbol_id);
#ifdef __cplusplus
}
#endif
#endif
