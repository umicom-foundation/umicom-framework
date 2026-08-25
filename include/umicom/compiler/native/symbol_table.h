/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/symbol_table.h
 *
 * PURPOSE:
 *   Provide scope-aware symbol registration and lexical name lookup with deterministic shadowing semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SYMBOL_TABLE_H
#define UMICOM_COMPILER_NATIVE_SYMBOL_TABLE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/scope.h"
#include "umicom/compiler/native/symbol.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeSymbolTable { UmiNativeSymbol symbols[UMI_NC_MAX_SYMBOLS]; size_t symbol_count; UmiNativeScope scopes[UMI_NC_MAX_SCOPES]; size_t scope_count; uint64_t revision; } UmiNativeSymbolTable;
void umi_nc_symbol_table_init(UmiNativeSymbolTable *table);
UmiStatus umi_nc_symbol_table_add_scope(UmiNativeSymbolTable *table,const UmiNativeScope *scope);
UmiStatus umi_nc_symbol_table_add_symbol(UmiNativeSymbolTable *table,const UmiNativeSymbol *symbol);
const UmiNativeScope *umi_nc_symbol_table_scope(const UmiNativeSymbolTable *table,uint32_t scope_id);
const UmiNativeSymbol *umi_nc_symbol_table_lookup(const UmiNativeSymbolTable *table,uint32_t scope_id,const char *name);
#ifdef __cplusplus
}
#endif
#endif
