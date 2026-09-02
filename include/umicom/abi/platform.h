/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/platform.h
 *
 * PURPOSE:
 *   Describe the architecture and operating-system constraints of one binary contract.
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
#ifndef UMICOM_ABI_PLATFORM_H
#define UMICOM_ABI_PLATFORM_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi platform data shared with callers of this public contract.
 */
typedef struct UmiAbiPlatform { const char *os; const char *architecture; size_t pointer_size; int little_endian; } UmiAbiPlatform;
/**
 * Provide the abi platform current operation used by this module and its client
 * applications.
 */
UmiAbiPlatform umi_abi_platform_current(void);
/**
 * Check that abi platform satisfies its contract before another service relies on it.
 */
UmiStatus umi_abi_platform_validate(const UmiAbiPlatform *platform);
/**
 * Provide the abi platform compatible operation used by this module and its client
 * applications.
 */
int umi_abi_platform_compatible(const UmiAbiPlatform *required, const UmiAbiPlatform *available);
#ifdef __cplusplus
}
#endif
#endif
