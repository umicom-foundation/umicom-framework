/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/symbol.h
 *
 * PURPOSE:
 *   Describe one stable exported C symbol that is part of an ABI surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_SYMBOL_H
#define UMICOM_ABI_SYMBOL_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiSymbol { const char *name; uint32_t since_abi; int required; } UmiAbiSymbol;
UmiStatus umi_abi_symbol_validate(const UmiAbiSymbol *symbol);
#ifdef __cplusplus
}
#endif
#endif
