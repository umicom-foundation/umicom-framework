/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/relocation.h
 *
 * PURPOSE:
 *   Represent object relocation requests independently from ELF, COFF and Mach-O writers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_RELOCATION_H
#define UMICOM_COMPILER_NATIVE_RELOCATION_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native relocation kind values accepted by this public contract.
 */
typedef enum UmiNativeRelocationKind { UMI_NC_RELOC_ABS64=1, UMI_NC_RELOC_PC32=2, UMI_NC_RELOC_CALL=3, UMI_NC_RELOC_GOT=4 } UmiNativeRelocationKind;
/**
 * Represent the native relocation data shared with callers of this public contract.
 */
typedef struct UmiNativeRelocation { UmiNativeRelocationKind kind; char section[UMI_NC_NAME_CAPACITY]; uint64_t offset; char symbol[UMI_NC_NAME_CAPACITY]; int64_t addend; } UmiNativeRelocation;
/**
 * Initialise nc relocation from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_relocation_init(UmiNativeRelocation *relocation,UmiNativeRelocationKind kind,const char *section,uint64_t offset,const char *symbol,int64_t addend);
/**
 * Check that nc relocation satisfies its contract before another service relies on it.
 */
UmiStatus umi_nc_relocation_validate(const UmiNativeRelocation *relocation,UmiNativeObjectFormat format);
#ifdef __cplusplus
}
#endif
#endif
