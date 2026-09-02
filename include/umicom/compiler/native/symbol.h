/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/symbol.h
 *
 * PURPOSE:
 *   Describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_SYMBOL_H
#define UMICOM_COMPILER_NATIVE_SYMBOL_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native symbol kind values accepted by this public contract.
 */
typedef enum UmiNativeSymbolKind { UMI_NC_SYMBOL_VARIABLE=1, UMI_NC_SYMBOL_FUNCTION=2, UMI_NC_SYMBOL_TYPE=3, UMI_NC_SYMBOL_ENUMERATOR=4, UMI_NC_SYMBOL_LABEL=5 } UmiNativeSymbolKind;
/**
 * Represent the native symbol data shared with callers of this public contract.
 */
typedef struct UmiNativeSymbol { uint32_t id; UmiNativeSymbolKind kind; char name[UMI_NC_NAME_CAPACITY]; uint32_t type_id; uint32_t scope_id; bool defined; bool external_linkage; bool internal_linkage; } UmiNativeSymbol;
/**
 * Initialise nc symbol from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_symbol_init(UmiNativeSymbol *symbol,uint32_t id,UmiNativeSymbolKind kind,const char *name,uint32_t type_id,uint32_t scope_id);
/**
 * Check that nc symbol satisfies its contract before another service relies on it.
 */
UmiStatus umi_nc_symbol_validate(const UmiNativeSymbol *symbol);
#ifdef __cplusplus
}
#endif
#endif
