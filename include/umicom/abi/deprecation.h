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

/* BEGINNER NOTE:
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_DEPRECATION_H
#define UMICOM_ABI_DEPRECATION_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiDeprecation { const char *symbol; uint32_t deprecated_since; uint32_t remove_after; const char *replacement; } UmiAbiDeprecation;
UmiStatus umi_abi_deprecation_validate(const UmiAbiDeprecation *entry);
int umi_abi_deprecation_removable(const UmiAbiDeprecation *entry, uint32_t current_abi);
#ifdef __cplusplus
}
#endif
#endif
