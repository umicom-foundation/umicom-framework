/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/binary.h
 *
 * PURPOSE:
 *   Describe a built native artifact for architecture and ABI compatibility checks.
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
#ifndef UMICOM_ABI_BINARY_H
#define UMICOM_ABI_BINARY_H
#include <stdint.h>
#include "umicom/abi/platform.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiBinary { const char *name; UmiAbiPlatform platform; uint32_t abi_version; uint64_t contract_fingerprint; } UmiAbiBinary;
UmiStatus umi_abi_binary_validate(const UmiAbiBinary *binary);
int umi_abi_binary_compatible(const UmiAbiBinary *consumer, const UmiAbiBinary *provider);
#ifdef __cplusplus
}
#endif
#endif
