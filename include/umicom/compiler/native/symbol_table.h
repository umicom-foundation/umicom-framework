/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/symbol_table.h
 *
 * PURPOSE:
 *   Provide scope-aware symbol registration and lexical name lookup with deterministic shadowing semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the native symbol table data shared with callers of this public contract.
 */
typedef struct UmiNativeSymbolTable { UmiNativeSymbol symbols[UMI_NC_MAX_SYMBOLS]; size_t symbol_count; UmiNativeScope scopes[UMI_NC_MAX_SCOPES]; size_t scope_count; uint64_t revision; } UmiNativeSymbolTable;
/**
 * Initialise nc symbol table from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_symbol_table_init(UmiNativeSymbolTable *table);
/**
 * Provide the nc symbol table add scope operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_symbol_table_add_scope(UmiNativeSymbolTable *table,const UmiNativeScope *scope);
/**
 * Provide the nc symbol table add symbol operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_symbol_table_add_symbol(UmiNativeSymbolTable *table,const UmiNativeSymbol *symbol);
/**
 * Provide the nc symbol table scope operation used by this module and its client
 * applications.
 */
const UmiNativeScope *umi_nc_symbol_table_scope(const UmiNativeSymbolTable *table,uint32_t scope_id);
/**
 * Find nc symbol table while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiNativeSymbol *umi_nc_symbol_table_lookup(const UmiNativeSymbolTable *table,uint32_t scope_id,const char *name);
#ifdef __cplusplus
}
#endif
#endif
