/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/deprecation.h
 *
 * PURPOSE:
 *   Track public APIs scheduled for retirement without silently breaking consumers.
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
#ifndef UMICOM_ABI_DEPRECATION_H
#define UMICOM_ABI_DEPRECATION_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the abi deprecation data shared with callers of this public contract.
 */
typedef struct UmiAbiDeprecation { const char *symbol; uint32_t deprecated_since; uint32_t remove_after; const char *replacement; } UmiAbiDeprecation;
/**
 * Check that abi deprecation satisfies its contract before another service relies on it.
 */
UmiStatus umi_abi_deprecation_validate(const UmiAbiDeprecation *entry);
/**
 * Provide the abi deprecation removable operation used by this module and its client
 * applications.
 */
int umi_abi_deprecation_removable(const UmiAbiDeprecation *entry, uint32_t current_abi);
#ifdef __cplusplus
}
#endif
#endif
