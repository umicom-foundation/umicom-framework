/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/symbol.h
 *
 * PURPOSE:
 *   Describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiNativeSymbolKind { UMI_NC_SYMBOL_VARIABLE=1, UMI_NC_SYMBOL_FUNCTION=2, UMI_NC_SYMBOL_TYPE=3, UMI_NC_SYMBOL_ENUMERATOR=4, UMI_NC_SYMBOL_LABEL=5 } UmiNativeSymbolKind;
typedef struct UmiNativeSymbol { uint32_t id; UmiNativeSymbolKind kind; char name[UMI_NC_NAME_CAPACITY]; uint32_t type_id; uint32_t scope_id; bool defined; bool external_linkage; bool internal_linkage; } UmiNativeSymbol;
UmiStatus umi_nc_symbol_init(UmiNativeSymbol *symbol,uint32_t id,UmiNativeSymbolKind kind,const char *name,uint32_t type_id,uint32_t scope_id);
UmiStatus umi_nc_symbol_validate(const UmiNativeSymbol *symbol);
#ifdef __cplusplus
}
#endif
#endif
