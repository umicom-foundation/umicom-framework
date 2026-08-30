/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/toolchain.h
 *
 * PURPOSE:
 *   Record compiler and architecture evidence associated with an SDK installation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_TOOLCHAIN_H
#define UMICOM_SDK_TOOLCHAIN_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkToolchainEvidence { const char *compiler_id; const char *compiler_version; const char *architecture; size_t pointer_size; } UmiSdkToolchainEvidence;
UmiStatus umi_sdk_toolchain_validate(const UmiSdkToolchainEvidence *toolchain);
#ifdef __cplusplus
}
#endif
#endif
