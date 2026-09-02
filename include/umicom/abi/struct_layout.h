/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/struct_layout.h
 *
 * PURPOSE:
 *   Record public-structure size and alignment so ABI drift can be detected explicitly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_STRUCT_LAYOUT_H
#define UMICOM_ABI_STRUCT_LAYOUT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi struct layout data shared with callers of this public contract.
 */
typedef struct UmiAbiStructLayout { const char *name; size_t size; size_t alignment; uint32_t abi_version; } UmiAbiStructLayout;
/**
 * Check that abi struct layout satisfies its contract before another service relies on it.
 */
UmiStatus umi_abi_struct_layout_validate(const UmiAbiStructLayout *layout);
/**
 * Provide the abi struct layout compatible operation used by this module and its client
 * applications.
 */
int umi_abi_struct_layout_compatible(const UmiAbiStructLayout *required, const UmiAbiStructLayout *available);
#ifdef __cplusplus
}
#endif
#endif
