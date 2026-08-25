/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/macro_table.h
 *
 * PURPOSE:
 *   Maintain a bounded macro catalogue with replaceable definitions and deterministic revision tracking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiNativeMacroTable { UmiNativeMacro items[UMI_NC_MAX_MACROS]; size_t count; uint64_t revision; } UmiNativeMacroTable;
void umi_nc_macro_table_init(UmiNativeMacroTable *table);
UmiStatus umi_nc_macro_table_define(UmiNativeMacroTable *table,const UmiNativeMacro *macro);
const UmiNativeMacro *umi_nc_macro_table_find(const UmiNativeMacroTable *table,const char *name);
UmiStatus umi_nc_macro_table_undef(UmiNativeMacroTable *table,const char *name);
#ifdef __cplusplus
}
#endif
#endif
