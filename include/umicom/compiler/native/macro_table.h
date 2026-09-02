/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/macro_table.h
 *
 * PURPOSE:
 *   Maintain a bounded macro catalogue with replaceable definitions and deterministic revision tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_MACRO_TABLE_H
#define UMICOM_COMPILER_NATIVE_MACRO_TABLE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/macro.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_MACROS 128U
/**
 * Represent the native macro table data shared with callers of this public contract.
 */
typedef struct UmiNativeMacroTable { UmiNativeMacro items[UMI_NC_MAX_MACROS]; size_t count; uint64_t revision; } UmiNativeMacroTable;
/**
 * Initialise nc macro table from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_macro_table_init(UmiNativeMacroTable *table);
/**
 * Provide the nc macro table define operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_macro_table_define(UmiNativeMacroTable *table,const UmiNativeMacro *macro);
/**
 * Find nc macro table while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiNativeMacro *umi_nc_macro_table_find(const UmiNativeMacroTable *table,const char *name);
/**
 * Provide the nc macro table undef operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_macro_table_undef(UmiNativeMacroTable *table,const char *name);
#ifdef __cplusplus
}
#endif
#endif
